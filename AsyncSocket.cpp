#include "stdafx.h"
#include "AsyncSocket.h"

CAsyncSocket::CAsyncSocket()
	: hGethost_(NULL)
	, soc_(INVALID_SOCKET)
	, bReady_(false)
	, bShutdown_(false)
	, hwnd_(NULL)
	, msg_(0)
	, name_(NULL)
	, port_(0)
	, bKeepSession_(false)
	, bDoHalfClose_(false)
{
}

CAsyncSocket::~CAsyncSocket()
{
	Close();
	delete [] name_;
}

// �񓯊��ʐM���J�n����
// ���łɊJ�n���Ă���Ƃ��͎��s���邪�Aname==NULL�̂Ƃ��͊J���Ă���\�P�b�g�ɑ��M�f�[�^��ǉ�����
bool CAsyncSocket::Send(HWND hwnd, UINT msg, const char *name, unsigned short port, const char *buf, int len, bool bKeepSession)
{
	if (len < 0) {
		len = lstrlenA(buf);
	}
	if (len > 0) {
		// �O�̃f�[�^�𑗐M�ς݂̂Ƃ��������M�f�[�^��ǉ��ł���
		if (IsPending() && !name && bKeepSession_ && sendBuf_.empty()) {
			sendBuf_.assign(&buf[0], &buf[len]);
			bKeepSession_ = bKeepSession;
			PostMessage(hwnd_, msg_, (WPARAM)soc_, WSAMAKESELECTREPLY(FD_WRITE, 0));
			return true;
		} else if (!IsPending() && name) {
			sendBuf_.assign(&buf[0], &buf[len]);
			hwnd_ = hwnd;
			msg_ = msg;
			delete [] name_;
			name_ = new char[lstrlenA(name) + 1];
			lstrcpyA(name_, name);
			port_ = port;
			bKeepSession_ = bKeepSession;
			// �L���[�ɂ��܂��Ă��邩������Ȃ����b�Z�[�W�𗬂����ߑҋ@
			bReady_ = true;
			PostMessage(hwnd, msg, 0, 0);
			return true;
		}
	}
	return false;
}

// �E�B���h�E���b�Z�[�W���������ăf�[�^����M����
// ��M�f�[�^��recvBuf�ɒǋL�����
// �߂�l: ���l=�ؒf����(-2=����,-1=���f), 0=����ɏ�������
int CAsyncSocket::ProcessRecv(WPARAM wParam, LPARAM lParam, std::vector<char> *recvBuf)
{
	UINT imAddr = INADDR_NONE;

	if (bReady_) {
		// �ҋ@��
		if (wParam || lParam) {
			return 0;
		}
		bReady_ = false;
		if (bShutdown_) {
			bShutdown_ = false;
			return -1;
		}
		if ((imAddr = inet_addr(name_)) == INADDR_NONE) {
			hGethost_ = WSAAsyncGetHostByName(hwnd_, msg_, name_, hostBuf_, sizeof(hostBuf_));
			return hGethost_ ? 0 : -1;
		}
		// IP�A�h���X���l(���O�������ȗ�)

	} else if (hGethost_) {
		// ���O������
		bool bValid = wParam == (WPARAM)hGethost_ && WSAGETASYNCERROR(lParam) == 0;
		hGethost_ = NULL;
		if (bShutdown_) {
			bShutdown_ = false;
			return -1;
		}
		if (!bValid || (imAddr = *(UINT*)((HOSTENT*)hostBuf_)->h_addr) == INADDR_NONE) {
			return -1;
		}
	}

	if (imAddr != INADDR_NONE) {
		// �ڑ�
		if ((soc_ = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
			return -1;
		}
		WSAAsyncSelect(soc_, hwnd_, msg_, FD_WRITE | FD_READ | FD_CLOSE);
		struct sockaddr_in addr = {0};
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = imAddr;
		addr.sin_port = htons(port_);
		if (connect(soc_, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {
			closesocket(soc_);
			soc_ = INVALID_SOCKET;
			return -1;
		}
		return 0;
	}

	if (soc_ != INVALID_SOCKET) {
		// ����M��
		if (wParam == (WPARAM)soc_) {
			switch(WSAGETSELECTEVENT(lParam)) {
			case FD_WRITE:
				if (!bShutdown_) {
					while (!sendBuf_.empty()) {
						int wrote = send(soc_, &sendBuf_.front(), (int)sendBuf_.size(), 0);
						if (wrote == SOCKET_ERROR) {
							// WSAEWOULDBLOCK���͂���FD_WRITE�ɐ摗��
							if (WSAGetLastError() != WSAEWOULDBLOCK) {
								Shutdown();
							}
							break;
						}
						sendBuf_.erase(sendBuf_.begin(), sendBuf_.begin() + wrote);
					}
					if (sendBuf_.empty() && !bKeepSession_ && bDoHalfClose_) {
						// �n�[�t�N���[�Y
						// Bitdefender���Ŏ�M�����f����s����m�F(2013-10-28)
						shutdown(soc_, SD_SEND);
					}
				}
				return 0;
			case FD_READ:
				for (;;) {
					char buf[2048];
					int read = recv(soc_, buf, sizeof(buf), 0);
					if (read == SOCKET_ERROR || read <= 0) {
						// FD_CLOSE�ŏE���̂Ŗ���
						return 0;
					}
					recvBuf->insert(recvBuf->end(), &buf[0], &buf[read]);
				}
				break;
			case FD_CLOSE:
				if (WSAGETSELECTERROR(lParam) != 0) {
					Close();
					return -1;
				}
				for (;;) {
					char buf[2048];
					int read = recv(soc_, buf, sizeof(buf), 0);
					if (read == SOCKET_ERROR || read <= 0) {
						Close();
						return read == 0 ? -2 : -1;
					}
					recvBuf->insert(recvBuf->end(), &buf[0], &buf[read]);
				}
				break;
			}
		}
		return 0;
	}
	return -1;
}

// ����M��~��v������
// �Ăяo����ProcessRecv()�����l��Ԃ��Ɗ���(�\�P�b�g��������)
bool CAsyncSocket::Shutdown()
{
	if (IsPending() && !bShutdown_) {
		if (soc_ != INVALID_SOCKET) {
			shutdown(soc_, SD_BOTH);
		}
		bShutdown_ = true;
	}
	return bShutdown_;
}

// �\�P�b�g�������I�ɕ���
// �|�X�g���ꂽ���b�Z�[�W���c��\��������̂łȂ�ׂ�Shutdown()���g��
void CAsyncSocket::Close()
{
	if (hGethost_) {
		WSACancelAsyncRequest(hGethost_);
		hGethost_ = NULL;
	}
	if (soc_ != INVALID_SOCKET) {
		closesocket(soc_);
		soc_ = INVALID_SOCKET;
	}
	bReady_ = false;
	bShutdown_ = false;
}
