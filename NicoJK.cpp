/*
	NicoJK
		TVTest �j�R�j�R�����v���O�C��
*/

#include "stdafx.h"
#include "Util.h"
#include "AsyncSocket.h"
#include "TextFileReader.h"
#include "CommentWindow.h"
#define TVTEST_PLUGIN_CLASS_IMPLEMENT
#include "TVTestPlugin.h"
#include "resource.h"
#include "NetworkServiceIDTable.h"
#include "JKIDNameTable.h"
#include "NicoJK.h"

//NicoJKKakoLog
#include "NicoJKKakolog/NicoJKKakolog.h"
#pragma comment(lib, "comctl32.lib")
//NicoJKKakoLog


#ifdef _DEBUG
#include <stdarg.h>
inline void dprintf_real( const _TCHAR * fmt, ... )
{
  _TCHAR buf[1024];
  va_list ap;
  va_start(ap, fmt);
  _vsntprintf_s(buf, 1024, fmt, ap);
  va_end(ap);
  OutputDebugString(buf);
}
#  define dprintf dprintf_real
#else
#  define dprintf __noop
#endif

// �ʐM�p
#define WMS_FORCE (WM_APP + 101)
#define WMS_JK (WM_APP + 102)
#define WMS_POST (WM_APP + 103)

#define WM_RESET_STREAM (WM_APP + 105)
#define WM_UPDATE_LIST (WM_APP + 106)
#define WM_SET_ZORDER (WM_APP + 107)
#define WM_POST_COMMENT (WM_APP + 108)

#define JK_HOST_NAME "jk.nicovideo.jp"

enum {
	TIMER_UPDATE = 1,
	TIMER_JK_WATCHDOG,
	TIMER_FORWARD,
	TIMER_SETUP_CURJK,
	TIMER_OPEN_DROPFILE,
	TIMER_DONE_MOVE,
	TIMER_DONE_SIZE,
	TIMER_DONE_POSCHANGE,
};

enum {
	COMMAND_HIDE_FORCE,
	COMMAND_HIDE_COMMENT,
	COMMAND_FORWARD_A,
};

bool CNicoJK::RPL_ELEM::AssignFromPattern()
{
	static const std::regex reBrace("[Ss](.)(.+?)\\1(.*?)\\1g");
	char utf8[_countof(pattern) * 3];
	int len = WideCharToMultiByte(CP_UTF8, 0, pattern, -1, utf8, _countof(utf8) - 1, NULL, NULL);
	utf8[len] = '\0';
	std::cmatch m;
	if (!std::regex_match(utf8, m, reBrace)) {
		return false;
	}
	try {
		re.assign(m[2].first, m[2].length());
	} catch (std::regex_error&) {
		return false;
	}
	fmt.assign(m[3].first, m[3].length());
	return true;
}

CNicoJK::CNicoJK()
	: bDragAcceptFiles_(false)
	, hForce_(NULL)
	, hKeyboardHook_(NULL)
	, bDisplayLogList_(false)
	, logListDisplayedSize_(0)
	, forwardTick_(0)
	, hSyncThread_(NULL)
	, bQuitSyncThread_(false)
	, bPendingTimerForward_(false)
	, bHalfSkip_(false)
	, bFlipFlop_(false)
	, forwardOffset_(0)
	, forwardOffsetDelta_(0)
	, currentJKToGet_(-1)
	, currentJK_(-1)
	, jkLeaveThreadCheck_(0)
	, bConnectedToCommentServer_(false)
	, commentServerResponseTick_(0)
	, bGetflvIsPremium_(false)
	, lastChatNo_(0)
	, lastPostTick_(0)
	, bRecording_(false)
	, bUsingLogfileDriver_(false)
	, bSetStreamCallback_(false)
	, currentLogfileJK_(-1)
	, hLogfile_(INVALID_HANDLE_VALUE)
	, hLogfileLock_(INVALID_HANDLE_VALUE)
	, currentReadLogfileJK_(-1)
	, tmReadLogText_(0)
	, readLogfileTick_(0)
	, pcr_(0)
	, pcrTick_(0)
	, pcrPid_(-1)
	, bSpecFile_(false)
	, dropFileTimeout_(0)
{
	szIniFileName_[0] = TEXT('\0');
	cookie_[0] = '\0';
	jkLeaveThreadID_[0] = '\0';
	commentServerResponse_[0] = '\0';
	getflvUserID_[0] = '\0';
	lastPostComm_[0] = TEXT('\0');
	readLogText_[0] = '\0';
	tmpSpecFileName_[0] = TEXT('\0');
	dropFileName_[0] = TEXT('\0');
	memset(&s_, 0, sizeof(s_));
	// TOT���擾�ł��Ă��Ȃ����Ƃ�\��
	ftTot_[0].dwHighDateTime = 0xFFFFFFFF;
	pcrPids_[0] = -1;
}

bool CNicoJK::GetPluginInfo(TVTest::PluginInfo *pInfo)
{
	// �v���O�C���̏���Ԃ�
	pInfo->Type           = TVTest::PLUGIN_TYPE_NORMAL;
	pInfo->Flags          = 0;
	pInfo->pszPluginName  = L"NicoJK";
	pInfo->pszCopyright   = L"Public Domain";
	pInfo->pszDescription = L"�j�R�j�R������SDK�ŕ\��(�Đ��Ή�������)";
	return true;
}

bool CNicoJK::Initialize()
{
	// ����������
	if (!GetLongModuleFileName(g_hinstDLL, szIniFileName_, _countof(szIniFileName_)) ||
	    !PathRenameExtension(szIniFileName_, TEXT(".ini"))) {
		szIniFileName_[0] = TEXT('\0');
	}
	tmpSpecFileName_[0] = TEXT('\0');
	TCHAR path[MAX_PATH + 32];
	if (GetLongModuleFileName(g_hinstDLL, path, MAX_PATH)) {
		PathRemoveExtension(path);
		wsprintf(&path[lstrlen(path)], TEXT("_%u.tmp"), GetCurrentProcessId());
		if (lstrlen(path) < _countof(tmpSpecFileName_)) {
			lstrcpy(tmpSpecFileName_, path);
		}
	}
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
		return false;
	}
	// OsdCompositor�͑��v���O�C���Ƌ��p���邱�Ƃ�����̂ŁA�L���ɂ���Ȃ�Finalize()�܂Ŕj�����Ȃ�
	bool bEnableOsdCompositor = GetPrivateProfileInt(TEXT("Setting"), TEXT("enableOsdCompositor"), 0, szIniFileName_) != 0;
	if (!commentWindow_.Initialize(g_hinstDLL, &bEnableOsdCompositor)) {
		WSACleanup();
		return false;
	}
	if (bEnableOsdCompositor) {
		m_pApp->AddLog(L"OsdCompositor�����������܂����B");
	}
	// �R�}���h��o�^
	m_pApp->RegisterCommand(COMMAND_HIDE_FORCE, L"HideForce", L"�����E�B���h�E�̕\���ؑ�");
	m_pApp->RegisterCommand(COMMAND_HIDE_COMMENT, L"HideComment", L"�����R�����g�̕\���ؑ�");
	memset(s_.forwardList, 0, sizeof(s_.forwardList));
	for (int i = 0; i < _countof(s_.forwardList); ++i) {
		TCHAR key[16], name[32];
		wsprintf(key, TEXT("Forward%c"), TEXT('A') + i);
		wsprintf(name, TEXT("�����R�����g�̑O�i:%c"), TEXT('A') + i);
		if ((s_.forwardList[i] = GetPrivateProfileInt(TEXT("Setting"), key, INT_MAX, szIniFileName_)) == INT_MAX) {
			break;
		}
		m_pApp->RegisterCommand(COMMAND_FORWARD_A + i, key, name);
	}
	// �C�x���g�R�[���o�b�N�֐���o�^
	m_pApp->SetEventCallback(EventCallback, this);


	//---NicoJKKakolog---
	nicoJKKakolog.Init(g_hinstDLL,m_pApp, szIniFileName_);
	//---NicoJKKakolog---
	return true;
}

bool CNicoJK::Finalize()
{
	// �I������
	if (m_pApp->IsPluginEnabled()) {
		TogglePlugin(false);
	}
	// �{�̂⑼�v���O�C���Ƃ̊���h�����߁A��U�L���ɂ���D&D�͍Ō�܂ňێ�����
	if (bDragAcceptFiles_) {
		DragAcceptFiles(m_pApp->GetAppWindow(), FALSE);
		bDragAcceptFiles_ = false;
	}
	commentWindow_.Finalize();
	WSACleanup();
	return true;
}

bool CNicoJK::TogglePlugin(bool bEnabled)
{
	if (bEnabled) {
		if (!hForce_) {
			LoadFromIni();
			// �l�b�g���[�N���ڑ��ł����O�t�H���_�ɂ���`�����l���𐨂����ɕ\���ł���悤�ɂ��邽��
			forceList_.clear();
			if (s_.logfileFolder[0]) {
				std::vector<WIN32_FIND_DATA> findList;
				TCHAR pattern[_countof(s_.logfileFolder) + 64];
				wsprintf(pattern, TEXT("%s\\jk*"), s_.logfileFolder);
				GetFindFileList(pattern, &findList);
				for (size_t i = 0; i < findList.size(); ++i) {
					FORCE_ELEM e;
					if (!StrCmpNI(findList[i].cFileName, TEXT("jk"), 2) && (e.jkID = StrToInt(&findList[i].cFileName[2])) > 0) {
						// �Ƃ肠�����g�ݍ��݂̃`�����l������ݒ肵�Ă���
						JKID_NAME_ELEM f;
						f.jkID = e.jkID;
						f.name = TEXT("");
						const JKID_NAME_ELEM *p = std::lower_bound(
							DEFAULT_JKID_NAME_TABLE, &DEFAULT_JKID_NAME_TABLE[_countof(DEFAULT_JKID_NAME_TABLE)], f, JKID_NAME_ELEM::COMPARE());
						if (p && p->jkID == f.jkID) {
							f.name = p->name;
						}
						lstrcpyn(e.name, f.name, _countof(e.name) - 1);
						lstrcat(e.name, TEXT("?"));
						e.force = 0;
						std::vector<FORCE_ELEM>::const_iterator it = forceList_.begin();
						for (; it != forceList_.end() && it->jkID < e.jkID; ++it);
						forceList_.insert(it, e);
					}
				}
			}
			// �K�v�Ȃ�T�[�o�ɓn��Cookie���擾
			cookie_[0] = '\0';
			TCHAR currDir[MAX_PATH];
			if (s_.execGetCookie[0] && GetLongModuleFileName(NULL, currDir, _countof(currDir)) && PathRemoveFileSpec(currDir)) {
				if (!GetProcessOutput(s_.execGetCookie, currDir, cookie_, _countof(cookie_), 10000)) {
					cookie_[0] = '\0';
					m_pApp->AddLog(L"execGetCookie�̎��s�Ɏ��s���܂����B");
				} else {
					// ���s->';'
					StrTrimA(cookie_, " \t\n\r");
					for (char *p = cookie_; *p; ++p) {
						if (*p == '\n' || *p == '\r') {
							*p = ';';
							if (*(p+1) == '\n') *++p = ' ';
						}
					}
				}
			}
			// �������쐬
			hForce_ = CreateDialogParam(g_hinstDLL, MAKEINTRESOURCE(IDD_FORCE), NULL,
			                            ForceDialogProc, reinterpret_cast<LPARAM>(this));
			if (hForce_) {
				// �E�B���h�E�R�[���o�b�N�֐���o�^
				m_pApp->SetWindowMessageCallback(WindowMsgCallback, this);
				// �X�g���[���R�[���o�b�N�֐���o�^(�w��t�@�C���Đ��@�\�̂��߂ɏ�ɓo�^)
				ToggleStreamCallback(true);
				// �L�[�{�[�h�t�b�N��o�^
				hKeyboardHook_ = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, g_hinstDLL, GetCurrentThreadId());
				// DWM�̍X�V�^�C�~���O��TIMER_FORWARD���ĂԃX���b�h���J�n(Vista�ȍ~)
				if (s_.timerInterval < 0) {
					OSVERSIONINFO vi;
					vi.dwOSVersionInfoSize = sizeof(vi);
					BOOL bEnabled;
					if (GetVersionEx(&vi) && vi.dwMajorVersion >= 6 && SUCCEEDED(DwmIsCompositionEnabled(&bEnabled)) && bEnabled) {
						bQuitSyncThread_ = false;
						hSyncThread_ = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, SyncThread, this, 0, NULL));
						if (hSyncThread_) {
							SetThreadPriority(hSyncThread_, THREAD_PRIORITY_ABOVE_NORMAL);
						}
					}
					if (!hSyncThread_) {
						m_pApp->AddLog(L"Aero�������̂��ߐݒ�timerInterval�̃��t���b�V�������@�\�̓I�t�ɂȂ�܂��B");
						SetTimer(hForce_, TIMER_FORWARD, 166667 / -s_.timerInterval, NULL);
					}
				}
				if (s_.dropLogfileMode != 0) {
					DragAcceptFiles(m_pApp->GetAppWindow(), TRUE);
					bDragAcceptFiles_ = true;
				}
			}
		}
		return hForce_ != NULL;
	} else {
		if (hForce_) {
			if (hSyncThread_) {
				bQuitSyncThread_ = true;
				WaitForSingleObject(hSyncThread_, INFINITE);
				CloseHandle(hSyncThread_);
				hSyncThread_ = NULL;
			}
			if (hKeyboardHook_) {
				UnhookWindowsHookEx(hKeyboardHook_);
				hKeyboardHook_ = NULL;
			}
			ToggleStreamCallback(false);
			m_pApp->SetWindowMessageCallback(NULL);
			DestroyWindow(hForce_);
			hForce_ = NULL;
			SaveToIni();
		}
		return true;
	}
}

LRESULT CALLBACK CNicoJK::KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	// Enter�L�[����
	if (code == HC_ACTION && wParam == VK_RETURN && !(lParam & 0x40000000)) {
		CNicoJK *pThis = dynamic_cast<CNicoJK*>(g_pPlugin);
		// �t�H�[�J�X���R�����g���͗��ɂ���΃t�b�N
		if (pThis && IsChild(GetDlgItem(pThis->hForce_, IDC_CB_POST), GetFocus())) {
			// IME�ł�Enter�𖳎�����(�Q�l: https://github.com/rutice/chapter.auf )
			HIMC hImc = ImmGetContext(pThis->hForce_);
			bool bActive = ImmGetOpenStatus(hImc) && ImmGetCompositionString(hImc, GCS_COMPSTR, NULL, 0) > 0;
			ImmReleaseContext(pThis->hForce_, hImc);
			if (!bActive) {
				SendNotifyMessage(pThis->hForce_, WM_POST_COMMENT, 0, 0);
				return TRUE;
			}
		}
	}
	// Ctrl+'V'�L�[����
	else if (code == HC_ACTION && wParam == 'V' && GetKeyState(VK_CONTROL) < 0) {
		CNicoJK *pThis = dynamic_cast<CNicoJK*>(g_pPlugin);
		// �t�H�[�J�X���R�����g���͗��ɂ���΃t�b�N
		if (pThis && IsChild(GetDlgItem(pThis->hForce_, IDC_CB_POST), GetFocus())) {
			int len = GetWindowTextLength(GetDlgItem(pThis->hForce_, IDC_CB_POST));
			LONG selRange = static_cast<LONG>(SendDlgItemMessage(pThis->hForce_, IDC_CB_POST, CB_GETEDITSEL, NULL, NULL));
			// ���͗�����ɂȂ�Ƃ������t�b�N
			if (len == 0 || MAKELONG(0, len) == selRange) {
				// �N���b�v�{�[�h���擾
				TCHAR clip[512];
				clip[0] = TEXT('\0');
				if (OpenClipboard(NULL)) {
					HGLOBAL hg = GetClipboardData(CF_UNICODETEXT);
					if (hg) {
						LPWSTR pg = static_cast<LPWSTR>(GlobalLock(hg));
						if (pg) {
							lstrcpyn(clip, pg, _countof(clip));
							GlobalUnlock(hg);
						}
					}
					CloseClipboard();
				}
				// ���s->���R�[�h�Z�p���[�^
				LPTSTR q = clip;
				bool bLF = false;
				bool bMultiLine = false;
				for (LPCTSTR p = q; *p; ++p) {
					if (*p == TEXT('\n')) {
						*q++ = TEXT('\x1e');
						bLF = true;
					} else if (*p != TEXT('\r')) {
						*q++ = *p;
						bMultiLine = bLF;
					}
				}
				*q = TEXT('\0');
				// �t�b�N���K�v�Ȃ͕̂����s�̃y�[�X�g����
				if (bMultiLine) {
					SetDlgItemText(pThis->hForce_, IDC_CB_POST, clip);
					SendMessage(pThis->hForce_, WM_COMMAND, MAKEWPARAM(IDC_CB_POST, CBN_EDITCHANGE), 0);
					return TRUE;
				}
			}
		}
	}
	// ��1�����͖��������Ƃ̂���
	return CallNextHookEx(NULL, code, wParam, lParam);
}

unsigned int __stdcall CNicoJK::SyncThread(void *pParam)
{
	CNicoJK *pThis = static_cast<CNicoJK*>(pParam);
	DWORD count = 0;
	int timeout = 0;
	while (!pThis->bQuitSyncThread_) {
		if (FAILED(DwmFlush())) {
			// �r�W�[�Ɋׂ�Ȃ��悤��
			Sleep(500);
		}
		if (count >= 10000) {
			// �J���؂�Ȃ��ʂ̃��b�Z�[�W�𑗂�Ȃ�
			if (pThis->bPendingTimerForward_ && --timeout >= 0) {
				continue;
			}
			count -= 10000;
			timeout = 30;
			pThis->bPendingTimerForward_ = true;
			SendNotifyMessage(pThis->hForce_, WM_TIMER, TIMER_FORWARD, 0);
		}
		count += pThis->bHalfSkip_ ? -pThis->s_.timerInterval / 2 : -pThis->s_.timerInterval;
	}
	return 0;
}

void CNicoJK::ToggleStreamCallback(bool bSet)
{
	if (bSet) {
		if (!bSetStreamCallback_) {
			bSetStreamCallback_ = true;
			pcrPid_ = -1;
			pcrPids_[0] = -1;
			m_pApp->SetStreamCallback(0, StreamCallback, this);
		}
	} else {
		if (bSetStreamCallback_) {
			m_pApp->SetStreamCallback(TVTest::STREAM_CALLBACK_REMOVE, StreamCallback);
			bSetStreamCallback_ = false;
		}
	}
}

void CNicoJK::LoadFromIni()
{
	TCHAR *pBuf = NewGetPrivateProfileSection(TEXT("Setting"), szIniFileName_);
	s_.hideForceWindow		= GetBufferedProfileInt(pBuf, TEXT("hideForceWindow"), 0);
	s_.timerInterval		= GetBufferedProfileInt(pBuf, TEXT("timerInterval"), -5000);
	s_.halfSkipThreshold	= GetBufferedProfileInt(pBuf, TEXT("halfSkipThreshold"), 9999);
	s_.commentLineMargin	= GetBufferedProfileInt(pBuf, TEXT("commentLineMargin"), 125);
	s_.commentFontOutline	= GetBufferedProfileInt(pBuf, TEXT("commentFontOutline"), 0);
	s_.commentSize			= GetBufferedProfileInt(pBuf, TEXT("commentSize"), 100);
	s_.commentSizeMin		= GetBufferedProfileInt(pBuf, TEXT("commentSizeMin"), 16);
	s_.commentSizeMax		= GetBufferedProfileInt(pBuf, TEXT("commentSizeMax"), 9999);
	GetBufferedProfileString(pBuf, TEXT("commentFontName"), TEXT("�l�r �o�S�V�b�N"), s_.commentFontName, _countof(s_.commentFontName));
	GetBufferedProfileString(pBuf, TEXT("commentFontNameMulti"), TEXT("�l�r �o�S�V�b�N"), s_.commentFontNameMulti, _countof(s_.commentFontNameMulti));
	s_.bCommentFontBold		= GetBufferedProfileInt(pBuf, TEXT("commentFontBold"), 1) != 0;
	s_.bCommentFontAntiAlias = GetBufferedProfileInt(pBuf, TEXT("commentFontAntiAlias"), 1) != 0;
	s_.commentDuration		= GetBufferedProfileInt(pBuf, TEXT("commentDuration"), CCommentWindow::DISPLAY_DURATION);
	s_.logfileMode			= GetBufferedProfileInt(pBuf, TEXT("logfileMode"), 0);
	GetBufferedProfileString(pBuf, TEXT("logfileDrivers"),
	                         TEXT("BonDriver_UDP.dll:BonDriver_TCP.dll:BonDriver_File.dll:BonDriver_RecTask.dll:BonDriver_Pipe.dll"),
	                         s_.logfileDrivers, _countof(s_.logfileDrivers));
	GetBufferedProfileString(pBuf, TEXT("nonTunerDrivers"),
	                         TEXT("BonDriver_UDP.dll:BonDriver_TCP.dll:BonDriver_File.dll:BonDriver_RecTask.dll:BonDriver_Pipe.dll"),
	                         s_.nonTunerDrivers, _countof(s_.nonTunerDrivers));
	GetBufferedProfileString(pBuf, TEXT("execGetCookie"), TEXT(""), s_.execGetCookie, _countof(s_.execGetCookie));
	GetBufferedProfileString(pBuf, TEXT("mailDecorations"),
	                         TEXT("[cyan big]:[shita]:[green shita small]:[orange]::"),
	                         s_.mailDecorations, _countof(s_.mailDecorations));
	s_.bAnonymity			= GetBufferedProfileInt(pBuf, TEXT("anonymity"), 1) != 0;
	s_.bUseOsdCompositor	= GetBufferedProfileInt(pBuf, TEXT("useOsdCompositor"), 0) != 0;
	s_.bUseTexture			= GetBufferedProfileInt(pBuf, TEXT("useTexture"), 1) != 0;
	s_.bUseDrawingThread	= GetBufferedProfileInt(pBuf, TEXT("useDrawingThread"), 1) != 0;
	s_.bShowRadio			= GetBufferedProfileInt(pBuf, TEXT("showRadio"), 0) != 0;
	s_.bDoHalfClose			= GetBufferedProfileInt(pBuf, TEXT("doHalfClose"), 0) != 0;
	s_.maxAutoReplace		= GetBufferedProfileInt(pBuf, TEXT("maxAutoReplace"), 20);
	GetBufferedProfileString(pBuf, TEXT("abone"), TEXT("### NG ### &"), s_.abone, _countof(s_.abone));
	s_.dropLogfileMode		= GetBufferedProfileInt(pBuf, TEXT("dropLogfileMode"), 0);
	// �������O�t�H���_�̃p�X���쐬
	TCHAR path[MAX_PATH], dir[MAX_PATH];
	GetBufferedProfileString(pBuf, TEXT("logfileFolder"), TEXT("Plugins\\NicoJK"), path, _countof(path));
	if (path[0] && PathIsRelative(path)) {
		if (!GetLongModuleFileName(NULL, dir, _countof(dir)) || !PathRemoveFileSpec(dir) || !PathCombine(s_.logfileFolder, dir, path)) {
			s_.logfileFolder[0] = TEXT('\0');
		}
	} else {
		lstrcpy(s_.logfileFolder, path);
	}
	if (!PathIsDirectory(s_.logfileFolder)) {
		s_.logfileFolder[0] = TEXT('\0');
	}
	delete [] pBuf;

	pBuf = NewGetPrivateProfileSection(TEXT("Window"), szIniFileName_);
	s_.rcForce.left			= GetBufferedProfileInt(pBuf, TEXT("ForceX"), 0);
	s_.rcForce.top			= GetBufferedProfileInt(pBuf, TEXT("ForceY"), 0);
	s_.rcForce.right		= GetBufferedProfileInt(pBuf, TEXT("ForceWidth"), 0) + s_.rcForce.left;
	s_.rcForce.bottom		= GetBufferedProfileInt(pBuf, TEXT("ForceHeight"), 0) + s_.rcForce.top;
	s_.forceOpacity			= GetBufferedProfileInt(pBuf, TEXT("ForceOpacity"), 255);
	s_.commentOpacity		= GetBufferedProfileInt(pBuf, TEXT("CommentOpacity"), 255);
	s_.bSetRelative			= GetBufferedProfileInt(pBuf, TEXT("SetRelative"), 0) != 0;
	delete [] pBuf;

	ntsIDList_.clear();
	ntsIDList_.reserve(_countof(DEFAULT_NTSID_TABLE));
	for (int i = 0; i < _countof(DEFAULT_NTSID_TABLE); ++i) {
		NETWORK_SERVICE_ID_ELEM e = {DEFAULT_NTSID_TABLE[i]&~0xFFF0, DEFAULT_NTSID_TABLE[i]>>4&0xFFF};
		ntsIDList_.push_back(e);
	}
	// �ݒ�t�@�C���̃l�b�g���[�N/�T�[�r�XID-����ID�Ώƕ\���A�\�[�g���ێ����Ȃ���}�[�W
	pBuf = NewGetPrivateProfileSection(TEXT("Channels"), szIniFileName_);
	for (LPCTSTR p = pBuf; *p; p += lstrlen(p) + 1) {
		NETWORK_SERVICE_ID_ELEM e;
		if (_stscanf_s(p, TEXT("0x%x=%d"), &e.ntsID, &e.jkID) == 2) {
			// �ݒ�t�@�C���̒�`�ł͏�ʂƉ��ʂ��Ђ�����Ԃ��Ă���̂ŕ␳
			e.ntsID = (e.ntsID<<16) | (e.ntsID>>16);
			std::vector<NETWORK_SERVICE_ID_ELEM>::iterator it =
				std::lower_bound(ntsIDList_.begin(), ntsIDList_.end(), e, NETWORK_SERVICE_ID_ELEM::COMPARE());
			if (it != ntsIDList_.end() && it->ntsID == e.ntsID) {
				*it = e;
			} else {
				ntsIDList_.insert(it, e);
			}
		}
	}
	delete [] pBuf;

	rplList_.clear();
	LoadRplListFromIni(TEXT("AutoReplace"), &rplList_);
	LoadRplListFromIni(TEXT("CustomReplace"), &rplList_);
}

void CNicoJK::SaveToIni()
{
	WritePrivateProfileInt(TEXT("Window"), TEXT("ForceX"), s_.rcForce.left, szIniFileName_);
	WritePrivateProfileInt(TEXT("Window"), TEXT("ForceY"), s_.rcForce.top, szIniFileName_);
	WritePrivateProfileInt(TEXT("Window"), TEXT("ForceWidth"), s_.rcForce.right - s_.rcForce.left, szIniFileName_);
	WritePrivateProfileInt(TEXT("Window"), TEXT("ForceHeight"), s_.rcForce.bottom - s_.rcForce.top, szIniFileName_);
	WritePrivateProfileInt(TEXT("Window"), TEXT("ForceOpacity"), s_.forceOpacity, szIniFileName_);
	WritePrivateProfileInt(TEXT("Window"), TEXT("CommentOpacity"), s_.commentOpacity, szIniFileName_);
	WritePrivateProfileInt(TEXT("Window"), TEXT("SetRelative"), s_.bSetRelative, szIniFileName_);
}

void CNicoJK::LoadRplListFromIni(LPCTSTR section, std::vector<RPL_ELEM> *pRplList)
{
	TCHAR *pBuf = NewGetPrivateProfileSection(section, szIniFileName_);
	size_t lastSize = pRplList->size();
	for (LPCTSTR p = pBuf; *p; p += lstrlen(p) + 1) {
		RPL_ELEM e;
		if (!StrCmpNI(p, TEXT("Pattern"), 7) && StrToIntEx(&p[7], STIF_DEFAULT, &e.key)) {
			lstrcpyn(e.section, section, _countof(e.section));
			TCHAR key[32];
			wsprintf(key, TEXT("Comment%d"), e.key);
			GetBufferedProfileString(pBuf, key, TEXT(""), e.comment, _countof(e.comment));
			wsprintf(key, TEXT("Pattern%d"), e.key);
			GetBufferedProfileString(pBuf, key, TEXT(""), e.pattern, _countof(e.pattern));
			if (!e.AssignFromPattern()) {
				TCHAR text[64];
				wsprintf(text, TEXT("%s�̐��K�\�����ُ�ł��B"), key);
				m_pApp->AddLog(text);
			} else {
				pRplList->push_back(e);
			}
		}
	}
	delete [] pBuf;
	std::sort(pRplList->begin() + lastSize, pRplList->end(), RPL_ELEM::COMPARE());
}

void CNicoJK::SaveRplListToIni(LPCTSTR section, const std::vector<RPL_ELEM> &rplList, bool bClearSection)
{
	if (bClearSection) {
		WritePrivateProfileString(section, NULL, NULL, szIniFileName_);
	}
	std::vector<RPL_ELEM>::const_iterator it = rplList.begin();
	for (; it != rplList.end(); ++it) {
		if (!lstrcmpi(it->section, section)) {
			TCHAR key[32];
			wsprintf(key, TEXT("Pattern%d"), it->key);
			WritePrivateProfileString(section, key, it->pattern, szIniFileName_);
		}
	}
}

HWND CNicoJK::GetFullscreenWindow()
{
	TVTest::HostInfo hostInfo;
	if (m_pApp->GetFullscreen() && m_pApp->GetHostInfo(&hostInfo)) {
		wchar_t className[64];
		lstrcpynW(className, hostInfo.pszAppName, 48);
		lstrcatW(className, L" Fullscreen");

		HWND hwnd = NULL;
		while ((hwnd = FindWindowExW(NULL, hwnd, className, NULL)) != NULL) {
			DWORD pid;
			GetWindowThreadProcessId(hwnd, &pid);
			if (pid == GetCurrentProcessId()) {
				return hwnd;
			}
		}
	}
	return NULL;
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	void **params = reinterpret_cast<void**>(lParam);
	TCHAR className[64];
	if (GetClassName(hwnd, className, _countof(className)) && !lstrcmp(className, static_cast<LPCTSTR>(params[1]))) {
		// ��������
		*static_cast<HWND*>(params[0]) = hwnd;
		return FALSE;
	}
	return TRUE;
}

// TVTest��Video Container�E�B���h�E��T��
HWND CNicoJK::FindVideoContainer()
{
	HWND hwndFound = NULL;
	TVTest::HostInfo hostInfo;
	if (m_pApp->GetHostInfo(&hostInfo)) {
		TCHAR searchName[64];
		lstrcpyn(searchName, hostInfo.pszAppName, 32);
		lstrcat(searchName, L" Video Container");

		void *params[2] = { &hwndFound, searchName };
		HWND hwndFull = GetFullscreenWindow();
		EnumChildWindows(hwndFull ? hwndFull : m_pApp->GetAppWindow(), EnumWindowsProc, reinterpret_cast<LPARAM>(params));
	}
	return hwndFound;
}

// �Đ����̃X�g���[���̃l�b�g���[�N/�T�[�r�XID���擾����
DWORD CNicoJK::GetCurrentNetworkServiceID()
{
	TVTest::ServiceInfo si;
	int index = m_pApp->GetService();
	if (index >= 0 && m_pApp->GetServiceInfo(index, &si)) {
		TVTest::ChannelInfo ci;
		
		if (m_pApp->GetCurrentChannelInfo(&ci) && ci.NetworkID) {
			if (0x7880 <= ci.NetworkID && ci.NetworkID <= 0x7FEF) {
				// �n��g�̃T�[�r�X��ʂƃT�[�r�X�ԍ��̓}�X�N����
				return (static_cast<DWORD>(si.ServiceID&~0x0187) << 16) | 0x000F;
			}
			return (static_cast<DWORD>(si.ServiceID) << 16) | ci.NetworkID;
		}
		// �`�����l���X�L�������Ă��Ȃ���GetCurrentChannelInfo()���l�b�g���[�NID�̎擾�Ɏ��s����悤
		if (si.ServiceID >= 0x0400) {
			// �n��g���ۂ��̂Ń}�X�N����
			return (static_cast<DWORD>(si.ServiceID&~0x0187) << 16) | 0;
		}
		return (static_cast<DWORD>(si.ServiceID) << 16) | 0;
	}
	return 0;
}

// �Đ����̃X�g���[����TOT����(�擾����̌o�ߎ��Ԃŕ␳�ς�)��UTC�Ŏ擾����
bool CNicoJK::GetCurrentTot(FILETIME *pft)
{
	CBlockLock lock(&streamLock_);
	DWORD tick = GetTickCount();
	if (ftTot_[0].dwHighDateTime == 0xFFFFFFFF) {
		// TOT���擾�ł��Ă��Ȃ�
		return false;
	} else if (tick - pcrTick_ >= 2000) {
		// 2�b�ȏ�PCR���擾�ł��Ă��Ȃ����|�[�Y��?
		*pft = ftTot_[0];
		return true;
	} else if (ftTot_[1].dwHighDateTime == 0xFFFFFFFF) {
		// �Đ����x�͕�����Ȃ�
		*pft = ftTot_[0];
		*pft += (tick - totTick_[0]) * FILETIME_MILLISECOND;
		return true;
	} else {
		DWORD delta = totTick_[0] - totTick_[1];
		// �Đ����x(10%�`1000%)
		LONGLONG speed = !delta ? FILETIME_MILLISECOND : (ftTot_[0] - ftTot_[1]) / delta;
		speed = min(max(speed, FILETIME_MILLISECOND / 10), FILETIME_MILLISECOND * 10);
		*pft = ftTot_[0];
		*pft += (tick - totTick_[0]) * speed;
		return true;
	}
}

// ���݂�BonDriver��':'��؂�̃��X�g�Ɋ܂܂�邩�ǂ������ׂ�
bool CNicoJK::IsMatchDriverName(LPCTSTR drivers)
{
	TCHAR path[MAX_PATH];
	m_pApp->GetDriverName(path, _countof(path));
	LPCTSTR name = PathFindFileName(path);
	int len = lstrlen(name);
	if (len > 0) {
		for (LPCTSTR p = drivers; (p = StrStrI(p, name)) != NULL; p += len) {
			if ((p == drivers || p[-1] == TEXT(':')) && (p[len] == TEXT('\0') || p[len] == TEXT(':'))) {
				return true;
			}
		}
	}
	return false;
}

// �w�肵������ID�̃��O�t�@�C���ɏ�������
// jkID�����l�̂Ƃ��̓��O�t�@�C�������
void CNicoJK::WriteToLogfile(int jkID, const char *text)
{
	if (!s_.logfileFolder[0] || s_.logfileMode == 0 || s_.logfileMode == 1 && !bRecording_) {
		// ���O���L�^���Ȃ�
		jkID = -1;
	}
	if (currentLogfileJK_ >= 0 && currentLogfileJK_ != jkID) {
		// ����
		CloseHandle(hLogfile_);
		CloseHandle(hLogfileLock_);
		// ���b�N�t�@�C�����폜
		TCHAR lockPath[_countof(s_.logfileFolder) + 32];
		wsprintf(lockPath, TEXT("%s\\jk%d\\lockfile"), s_.logfileFolder, currentLogfileJK_);
		DeleteFile(lockPath);
		currentLogfileJK_ = -1;
		OutputMessageLog(TEXT("���O�t�@�C���̏������݂��I�����܂����B"));
	}
	if (currentLogfileJK_ < 0 && jkID >= 0) {
		unsigned int tm;
		TCHAR dir[_countof(s_.logfileFolder) + 32];
		wsprintf(dir, TEXT("%s\\jk%d"), s_.logfileFolder, jkID);
		if (GetChatDate(&tm, text) && (PathFileExists(dir) || CreateDirectory(dir, NULL))) {
			// ���b�N�t�@�C�����J��
			TCHAR lockPath[_countof(dir) + 32];
			wsprintf(lockPath, TEXT("%s\\lockfile"), dir);
			hLogfileLock_ = CreateFile(lockPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hLogfileLock_ != INVALID_HANDLE_VALUE) {
				// �J��
				TCHAR path[_countof(dir) + 32];
				wsprintf(path, TEXT("%s\\%010u.txt"), dir, tm);
				hLogfile_ = CreateFile(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hLogfile_ != INVALID_HANDLE_VALUE) {
					// �w�b�_����������(�ʂɖ����Ă�����)
					FILETIME ftUtc, ft;
					UnixTimeToFileTime(tm, &ftUtc);
					FileTimeToLocalFileTime(&ftUtc, &ft);
					SYSTEMTIME st;
					FileTimeToSystemTime(&ft, &st);
					char header[128];
					int len = wsprintfA(header, "<!-- NicoJK logfile from %04d-%02d-%02dT%02d:%02d:%02d -->\r\n",
					                    st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
					DWORD written;
					WriteFile(hLogfile_, header, len, &written, NULL);
					currentLogfileJK_ = jkID;

					TCHAR debug[_countof(path) + 64];
					wsprintf(debug, TEXT("���O�t�@�C��\"%s\"�̏������݂��J�n���܂����B"),
					         StrRChr(path, StrRChr(path, NULL, TEXT('\\')), TEXT('\\')) + 1);
					OutputMessageLog(debug);
				} else {
					CloseHandle(hLogfileLock_);
					DeleteFile(lockPath);
				}
			}
		}
	}
	// �J���Ă��珑������
	if (currentLogfileJK_ >= 0) {
		DWORD written;
		WriteFile(hLogfile_, text, lstrlenA(text), &written, NULL);
		WriteFile(hLogfile_, "\r\n", 2, &written, NULL);
	}
}

#define DWORD_MSB(x) ((x) & 0x80000000)

// �w�肵������ID�̎w�莞���̃��O1�s��ǂݍ���
// jkID�����l�̂Ƃ��̓��O�t�@�C�������
// jkID==0�͎w��t�@�C���Đ�(tmpSpecFileName_)��\������Ȏ���ID�Ƃ���
bool CNicoJK::ReadFromLogfile(int jkID, char *text, int textMax, unsigned int tmToRead)
{
	if (jkID != 0 && (!s_.logfileFolder[0] || !bUsingLogfileDriver_)) {
		// ���O��ǂ܂Ȃ�
		jkID = -1;
	}
	DWORD tick = GetTickCount();
	if (currentReadLogfileJK_ >= 0 && currentReadLogfileJK_ != jkID) {
		// ����
		readLogfile_.Close();
		readLogfileTick_ = tick;
		currentReadLogfileJK_ = -1;
		OutputMessageLog(TEXT("���O�t�@�C���̓ǂݍ��݂��I�����܂����B"));
	}
	if (!DWORD_MSB(tick - readLogfileTick_) && currentReadLogfileJK_ < 0 && jkID >= 0) {
		// �t�@�C���`�F�b�N���ʂɌJ�肩�����̂�h��
		readLogfileTick_ = tick + READ_LOG_FOLDER_INTERVAL;
		TCHAR path[_countof(s_.logfileFolder) + 64];
		path[0] = TEXT('\0');
		if (jkID == 0) {
			// �w��t�@�C���Đ�
			lstrcpyn(path, tmpSpecFileName_, _countof(path));
		} else {
			// jkID�̃��O�t�@�C���ꗗ�𖼑O���œ���
			std::vector<WIN32_FIND_DATA> findList;
			std::vector<LPWIN32_FIND_DATA> sortedList;
			TCHAR pattern[_countof(s_.logfileFolder) + 64];
			wsprintf(pattern, TEXT("%s\\jk%d\\??????????.txt"), s_.logfileFolder, jkID);
			GetFindFileList(pattern, &findList, &sortedList);
			// tmToRead�ȑO�ł����Ƃ��V�������O�t�@�C����T��
			WIN32_FIND_DATA findData;
			wsprintf(findData.cFileName, TEXT("%010u.txt"), tmToRead + (READ_LOG_FOLDER_INTERVAL / 1000 + 2));
			std::vector<LPWIN32_FIND_DATA>::const_iterator it =
				std::lower_bound(sortedList.begin(), sortedList.end(), &findData, LPWIN32_FIND_DATA_COMPARE());
			if (it != sortedList.begin()) {
				// ��������
				wsprintf(path, TEXT("%s\\jk%d\\%.14s"), s_.logfileFolder, jkID, (*(--it))->cFileName);
			}
		}
		if (path[0]) {
			if (readLogfile_.Open(path, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN)) {
				char last[CHAT_TAG_MAX];
				unsigned int tmLast;
				// �ŏI�s��tmToRead���ߋ��Ȃ�ǂމ��l����
				if (!readLogfile_.ReadLastLine(last, _countof(last)) || !GetChatDate(&tmLast, last) || tmLast < tmToRead) {
					// ����
					readLogfile_.Close();
				} else {
					// �܂�2���T��
					for (int scale = 2; ; scale *= 2) {
						char middle[CHAT_TAG_MAX];
						int sign = 0;
						for (;;) {
							if (!readLogfile_.ReadLine(middle, _countof(middle))) {
								break;
							}
							unsigned int tmMiddle;
							if (GetChatDate(&tmMiddle, middle)) {
								sign = tmMiddle + 10 > tmToRead ? -1 : 1;
								break;
							}
						}
						// �s�̎����������Ȃ����ŏ��̍s�����łɖ����Ȃ烊�Z�b�g
						if (sign == 0 || sign < 0 && scale == 2) {
							readLogfile_.ResetPointer();
							break;
						}
						int moveSize = readLogfile_.Seek(sign * scale);
						dprintf(TEXT("CNicoJK::ReadFromLogfile() moveSize=%d\n"), moveSize); // DEBUG
						// �ړ��ʂ��������Ȃ�Αł��؂�
						if (-32 * 1024 < moveSize && moveSize < 32 * 1024) {
							// tmToRead�����m���ɉߋ��ɂȂ�ʒu�܂Ŗ߂�
							readLogfile_.Seek(-scale);
							// �V�[�N����̒��r���[��1�s��ǂݔ�΂�
							readLogfile_.ReadLine(middle, 1);
							break;
						}
						readLogfile_.ReadLine(middle, 1);
					}
					// tmToRead���ߋ��̍s��ǂݔ�΂�
					for (;;) {
						if (!readLogfile_.ReadLine(readLogText_, _countof(readLogText_))) {
							// ����
							readLogfile_.Close();
							break;
						} else if (GetChatDate(&tmReadLogText_, readLogText_) && tmReadLogText_ > tmToRead/*>=�̓_��*/) {
							currentReadLogfileJK_ = jkID;

							TCHAR debug[_countof(path) + 64];
							wsprintf(debug, TEXT("���O�t�@�C��\"jk%d\\%s\"�̓ǂݍ��݂��J�n���܂����B"), jkID, PathFindFileName(path));
							OutputMessageLog(debug);
							break;
						}
					}
				}
			}
		}
	}
	bool bRet = false;
	// �J���Ă���ǂݍ���
	if (currentReadLogfileJK_ >= 0) {
		if (readLogText_[0] && tmReadLogText_ <= tmToRead) {
			lstrcpynA(text, readLogText_, textMax);
			readLogText_[0] = '\0';
			bRet = true;
		}
		if (!readLogText_[0]) {
			for (;;) {
				if (!readLogfile_.ReadLine(readLogText_, _countof(readLogText_))) {
					// ����
					readLogfile_.Close();
					readLogfileTick_ = tick;
					currentReadLogfileJK_ = -1;
					OutputMessageLog(TEXT("���O�t�@�C���̓ǂݍ��݂��I�����܂����B"));
					break;
				} else if (GetChatDate(&tmReadLogText_, readLogText_)) {
					break;
				}
			}
		}
	}
	return bRet;
}

static int GetWindowHeight(HWND hwnd)
{
	RECT rc;
	return hwnd && GetWindowRect(hwnd, &rc) ? rc.bottom - rc.top : 0;
}

// �C�x���g�R�[���o�b�N�֐�
// �����C�x���g���N����ƌĂ΂��
LRESULT CALLBACK CNicoJK::EventCallback(UINT Event, LPARAM lParam1, LPARAM lParam2, void *pClientData)
{
	CNicoJK *pThis = static_cast<CNicoJK*>(pClientData);
	switch (Event) {
	case TVTest::EVENT_PLUGINENABLE:
		// �v���O�C���̗L����Ԃ��ω�����
		return pThis->TogglePlugin(lParam1 != 0);
	case TVTest::EVENT_RECORDSTATUSCHANGE:
		// �^���Ԃ��ω�����
		pThis->bRecording_ = lParam1 != TVTest::RECORD_STATUS_NOTRECORDING;
		break;
	case TVTest::EVENT_FULLSCREENCHANGE:
		// �S��ʕ\����Ԃ��ω�����
		if (pThis->m_pApp->IsPluginEnabled()) {
			// �I�[�i�[���ς��̂ŃR�����g�E�B���h�E�����Ȃ���
			pThis->commentWindow_.Destroy();
			if (pThis->commentWindow_.GetOpacity() != 0 && pThis->m_pApp->GetPreview()) {
				HWND hwnd = pThis->FindVideoContainer();
				pThis->commentWindow_.Create(hwnd);
				pThis->bHalfSkip_ = GetWindowHeight(hwnd) >= pThis->s_.halfSkipThreshold;
			}
			// �S��ʑJ�ڎ��͉B�ꂽ�ق����g�����肪�����̂ŌĂ΂Ȃ�
			if (!lParam1) {
				SendMessage(pThis->hForce_, WM_SET_ZORDER, 0, 0);
			}
		}
		break;
	case TVTest::EVENT_PREVIEWCHANGE:
		// �v���r���[�\����Ԃ��ω�����
		if (pThis->m_pApp->IsPluginEnabled()) {
			if (pThis->commentWindow_.GetOpacity() != 0 && lParam1 != 0) {
				HWND hwnd = pThis->FindVideoContainer();
				pThis->commentWindow_.Create(hwnd);
				pThis->bHalfSkip_ = GetWindowHeight(hwnd) >= pThis->s_.halfSkipThreshold;
				pThis->ProcessChatTag("<!--<chat date=\"0\" mail=\"cyan ue\" user_id=\"-\">(NicoJK ON)</chat>-->");
			} else {
				pThis->commentWindow_.Destroy();
			}
		}
		break;
	case TVTest::EVENT_DRIVERCHANGE:
		// �h���C�o���ύX���ꂽ
		if (pThis->m_pApp->IsPluginEnabled()) {
			pThis->bUsingLogfileDriver_ = pThis->IsMatchDriverName(pThis->s_.logfileDrivers);
		}
		// FALL THROUGH!
	case TVTest::EVENT_CHANNELCHANGE:
		// �`�����l�����ύX���ꂽ
		if (pThis->m_pApp->IsPluginEnabled()) {
			PostMessage(pThis->hForce_, WM_RESET_STREAM, 0, 0);
		}
		// FALL THROUGH!
	case TVTest::EVENT_SERVICECHANGE:
		// �T�[�r�X���ύX���ꂽ
		if (pThis->m_pApp->IsPluginEnabled()) {
			// �d����U�b�s���O�΍�̂��߃^�C�}�ŌĂ�
			SetTimer(pThis->hForce_, TIMER_SETUP_CURJK, SETUP_CURJK_DELAY, NULL);
		}
		break;
	case TVTest::EVENT_SERVICEUPDATE:
		// �T�[�r�X�̍\�����ω�����(�Đ��t�@�C����؂�ւ����Ƃ��Ȃ�)
		if (pThis->m_pApp->IsPluginEnabled()) {
			// ���[�U�̎����I�ȃ`�����l���ύX(EVENT_CHANNELCHANGE)�𑨂���̂���������
			// ��`���[�i�n��BonDriver���Ƃ���ł͕s�\���Ȃ���
			if (pThis->IsMatchDriverName(pThis->s_.nonTunerDrivers)) {
				SetTimer(pThis->hForce_, TIMER_SETUP_CURJK, SETUP_CURJK_DELAY, NULL);
			}
		}
		break;
	case TVTest::EVENT_COMMAND:
		// �R�}���h���I�����ꂽ
		if (pThis->m_pApp->IsPluginEnabled()) {
			switch (lParam1) {
			case COMMAND_HIDE_FORCE:
				if (IsWindowVisible(pThis->hForce_)) {
					ShowWindow(pThis->hForce_, SW_HIDE);
				} else {
					ShowWindow(pThis->hForce_, SW_SHOWNA);
				}
				SendMessage(pThis->hForce_, WM_UPDATE_LIST, TRUE, 0);
				SendMessage(pThis->hForce_, WM_SET_ZORDER, 0, 0);
				PostMessage(pThis->hForce_, WM_TIMER, TIMER_UPDATE, 0);
				break;
			case COMMAND_HIDE_COMMENT:
					if (pThis->commentWindow_.GetOpacity() == 0 && pThis->m_pApp->GetPreview()) {
						pThis->commentWindow_.ClearChat();
						HWND hwnd = pThis->FindVideoContainer();
						pThis->commentWindow_.Create(hwnd);
						pThis->bHalfSkip_ = GetWindowHeight(hwnd) >= pThis->s_.halfSkipThreshold;
						pThis->commentWindow_.AddChat(TEXT("(Comment ON)"), RGB(0x00,0xFF,0xFF), CCommentWindow::CHAT_POS_UE);
						// ��\���O�̕s�����x�𕜌�����
						BYTE newOpacity = static_cast<BYTE>(pThis->s_.commentOpacity>>8);
						pThis->commentWindow_.SetOpacity(newOpacity == 0 ? 255 : newOpacity);
					} else {
						pThis->commentWindow_.Destroy();
						// 8-15bit�ɔ�\���O�̕s�����x���L�����Ă���
						pThis->s_.commentOpacity = (pThis->s_.commentOpacity&~0xFF00) | (pThis->commentWindow_.GetOpacity()<<8);
						pThis->commentWindow_.SetOpacity(0);
					}
					SendDlgItemMessage(pThis->hForce_, IDC_SLIDER_OPACITY, TBM_SETPOS, TRUE, (pThis->commentWindow_.GetOpacity() * 10 + 254) / 255);
				break;
			default:
				if (COMMAND_FORWARD_A <= lParam1 && lParam1 < COMMAND_FORWARD_A + _countof(pThis->s_.forwardList)) {
					int forward = pThis->s_.forwardList[lParam1 - COMMAND_FORWARD_A];
					if (forward == 0) {
						pThis->forwardOffsetDelta_ = -pThis->forwardOffset_;
					} else {
						pThis->forwardOffsetDelta_ += forward;
					}
				}
				break;
			}
		}
		break;
	}
	return 0;
}

BOOL CALLBACK CNicoJK::WindowMsgCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *pResult, void *pUserData)
{
	CNicoJK *pThis = static_cast<CNicoJK*>(pUserData);
	switch (uMsg) {
	case WM_ACTIVATE:
		if (LOWORD(wParam) != WA_INACTIVE) {
			SendMessage(pThis->hForce_, WM_SET_ZORDER, 0, 0);
		}
		break;
	case WM_WINDOWPOSCHANGED:
		// WM_ACTIVATE����Ȃ�Z�I�[�_�[�̕ω��𑨂���B�t���X�N���[���ł��Ȃ��������Ă���̂Œ���
		SetTimer(pThis->hForce_, TIMER_DONE_POSCHANGE, 1000, NULL);
		break;
	case WM_MOVE:
		pThis->commentWindow_.OnParentMove();
		SetTimer(pThis->hForce_, TIMER_DONE_MOVE, 500, NULL);
		break;
	case WM_SIZE:
		pThis->commentWindow_.OnParentSize();
		SetTimer(pThis->hForce_, TIMER_DONE_SIZE, 500, NULL);
		break;
	case WM_DROPFILES:
		if (pThis->s_.dropLogfileMode == 0) {
			break;
		}
		if (pThis->m_pApp->GetFullscreen()) {
			// �t�@�C���_�C�A���O���ł�D&D�𖳎����邽��(�m���ł͂Ȃ�)
			HWND hwndActive = GetActiveWindow();
			if (hwndActive && (GetWindowLong(GetAncestor(hwndActive, GA_ROOT), GWL_EXSTYLE) & WS_EX_DLGMODALFRAME) != 0) {
				break;
			}
		}
		// �ǂݍ��݉\�Ȋg���q�����ŏ��ɂ݂������t�@�C�����J��
		pThis->dropFileTimeout_ = 0;
		for (UINT i = DragQueryFile(reinterpret_cast<HDROP>(wParam), 0xFFFFFFFF, NULL, 0); i != 0; --i) {
			if (DragQueryFile(reinterpret_cast<HDROP>(wParam), i - 1, pThis->dropFileName_, _countof(pThis->dropFileName_))) {
				LPCTSTR ext = PathFindExtension(pThis->dropFileName_);
				if (!lstrcmpi(ext, TEXT(".jkl")) || !lstrcmpi(ext, TEXT(".xml")) || !lstrcmpi(ext, TEXT(".txt"))) {
					if (pThis->bSpecFile_) {
						pThis->ReadFromLogfile(-1);
						DeleteFile(pThis->tmpSpecFileName_);
						pThis->bSpecFile_ = false;
					}
					SendDlgItemMessage(pThis->hForce_, IDC_CHECK_SPECFILE, BM_SETCHECK, BST_UNCHECKED, 0);
					if (pThis->s_.dropLogfileMode == 2) {
						// �E�B���h�E�̍��E�ǂ����D&D���ꂽ����Rel�`�F�b�N�{�b�N�X��ς���
						RECT rc;
						HWND hwndFull = pThis->GetFullscreenWindow();
						GetClientRect(hwndFull ? hwndFull : pThis->m_pApp->GetAppWindow(), &rc);
						POINT pt = {0};
						DragQueryPoint(reinterpret_cast<HDROP>(wParam), &pt);
						SendDlgItemMessage(pThis->hForce_, IDC_CHECK_RELATIVE, BM_SETCHECK, pt.x > rc.right / 2 ? BST_CHECKED : BST_UNCHECKED, 0);
					}
					bool bRel = SendDlgItemMessage(pThis->hForce_, IDC_CHECK_RELATIVE, BM_GETCHECK, 0, 0) == BST_CHECKED;
					pThis->dropFileTimeout_ = 10;
					SetTimer(pThis->hForce_, TIMER_OPEN_DROPFILE, bRel ? 2000 : 0, NULL);
					break;
				}
			}
		}
		// DragFinish()�����ɖ{�̂̃f�t�H���g�v���V�[�W���ɔC����
		break;
	}
	return FALSE;
}

// �R�����g(chat�^�O)1�s�����߂��ăR�����g�E�B���h�E�ɑ���
bool CNicoJK::ProcessChatTag(const char *tag, bool bShow, int showDelay)
{
	static const std::regex reChat("<chat(?= )(.*)>(.*?)</chat>");
	static const std::regex reMail(" mail=\"(.*?)\"");
	static const std::regex reAbone(" abone=\"1\"");
	static const std::regex reYourpost(" yourpost=\"1\"");
	static const std::regex reInsertAt(" insert_at=\"last\"");
	static const std::regex reAlign(" align=\"(left|right)");
	static const std::regex reUserID(" user_id=\"([0-9A-Za-z\\-_]{0,27})");
	static const std::regex reNo(" no=\"(\\d+)\"");
	// �u��
	std::string rpl[2];
	if (!rplList_.empty()) {
		rpl[1] = tag;
		std::vector<RPL_ELEM>::const_iterator it = rplList_.begin();
		for (int i = 0; it != rplList_.end(); ++it) {
			if (it->IsEnabled()) {
				try {
					rpl[i % 2] = std::regex_replace(rpl[(i + 1) % 2], it->re, it->fmt);
				} catch (std::regex_error&) {
					// �u���t�H�[�}�b�g�ُ�̂��ߖ�������
					continue;
				}
				tag = rpl[i++ % 2].c_str();
			}
		}
	}
	std::cmatch m, mm;
	unsigned int tm;
	if (std::regex_match(tag, m, reChat) && GetChatDate(&tm, tag)) {
		TCHAR text[CCommentWindow::CHAT_TEXT_MAX * 2];
		int len = MultiByteToWideChar(CP_UTF8, 0, m[2].first, static_cast<int>(m[2].length()), text, _countof(text) - 1);
		text[len] = TEXT('\0');
		DecodeEntityReference(text);
		// mail�����͖����Ƃ�������
		char mail[256];
		mail[0] = '\0';
		if (std::regex_search(m[1].first, m[1].second, mm, reMail)) {
			lstrcpynA(mail, mm[1].first, min(static_cast<int>(mm[1].length()) + 1, _countof(mail)));
		}
		// abone����(���[�J���g��)
		bool bAbone = std::regex_search(m[1].first, m[1].second, reAbone);
		if (bShow && !bAbone) {
			bool bYourpost = std::regex_search(m[1].first, m[1].second, reYourpost);
			// insert_at����(���[�J���g��)
			bool bInsertLast = std::regex_search(m[1].first, m[1].second, reInsertAt);
			// align����(���[�J���g��)
			CCommentWindow::CHAT_ALIGN align = CCommentWindow::CHAT_ALIGN_CENTER;
			if (std::regex_search(m[1].first, m[1].second, mm, reAlign)) {
				align = mm[1].first[0] == 'l' ? CCommentWindow::CHAT_ALIGN_LEFT : CCommentWindow::CHAT_ALIGN_RIGHT;
			}
			commentWindow_.AddChat(text, GetColor(mail), HasToken(mail, "shita") ? CCommentWindow::CHAT_POS_SHITA :
			                       HasToken(mail, "ue") ? CCommentWindow::CHAT_POS_UE : CCommentWindow::CHAT_POS_DEFAULT,
			                       HasToken(mail, "small") ? CCommentWindow::CHAT_SIZE_SMALL : CCommentWindow::CHAT_SIZE_DEFAULT,
			                       align, bInsertLast, bYourpost ? 160 : 0, showDelay);
		}

		// ���X�g�{�b�N�X�̃��O�\���L���[�ɒǉ�
		LOG_ELEM e;
		FILETIME ftUtc, ft;
		UnixTimeToFileTime(tm, &ftUtc);
		FileTimeToLocalFileTime(&ftUtc, &ft);
		FileTimeToSystemTime(&ft, &e.st);
		e.no = 0;
		e.marker[0] = TEXT('\0');
		if (!bShow) {
			lstrcpy(e.marker, TEXT("."));
		} else if (std::regex_search(m[1].first, m[1].second, mm, reUserID)) {
			len = MultiByteToWideChar(CP_UTF8, 0, mm[1].first, static_cast<int>(mm[1].length()), e.marker, _countof(e.marker) - 1);
			e.marker[len] = TEXT('\0');
			if (std::regex_search(m[1].first, m[1].second, mm, reNo)) {
				e.no = atoi(mm[1].first);
			}
		}
		if (bAbone) {
			lstrcpyn(e.text, s_.abone, _countof(e.text));
			int tail = lstrlen(e.text) - 1;
			if (tail >= 0 && e.text[tail] == TEXT('&')) {
				lstrcpyn(&e.text[tail], text, _countof(e.text) - tail);
			}
		} else {
			lstrcpyn(e.text, text, _countof(e.text));
		}
		logList_.push_back(e);
		return true;
	}
	return false;
}

// ���O�E�B���h�E�Ƀ��[�U�ւ̃��b�Z�[�W���O���o��
void CNicoJK::OutputMessageLog(LPCTSTR text)
{
	// ���X�g�{�b�N�X�̃��O�\���L���[�ɒǉ�
	LOG_ELEM e;
	GetLocalTime(&e.st);
	e.no = 0;
	lstrcpy(e.marker, TEXT("#"));
	lstrcpyn(e.text, text, _countof(e.text));
	logList_.push_back(e);
	if (hForce_) {
		SendMessage(hForce_, WM_UPDATE_LIST, FALSE, 0);
	}
}

// �R�����g���e���̕�������擾����
void CNicoJK::GetPostComboBoxText(LPTSTR comm, int commSize, LPTSTR mail, int mailSize)
{
	TCHAR text[512];
	if (!GetDlgItemText(hForce_, IDC_CB_POST, text, _countof(text))) {
		text[0] = TEXT('\0');
	}
	if (mail) {
		mail[0] = TEXT('\0');
	}
	// []�ň͂�ꂽ������mail�����l�Ƃ���
	LPCTSTR p = text;
	if (*p == '[') {
		p += StrCSpn(p, TEXT("]"));
		if (*p == ']') {
			if (mail) {
				lstrcpyn(mail, &text[1], min(static_cast<int>(p - text), mailSize));
			}
			++p;
		}
	}
	lstrcpyn(comm, p, commSize);
}

// �R�����g���e���̃��[�J���R�}���h����������
void CNicoJK::ProcessLocalPost(LPCTSTR comm)
{
	// �p�����[�^����
	TCHAR cmd[16];
	int cmdLen = StrCSpn(comm, TEXT(" "));
	lstrcpyn(cmd, comm, min(cmdLen + 1, _countof(cmd)));
	LPCTSTR arg = &comm[cmdLen] + StrSpn(&comm[cmdLen], TEXT(" "));
	int nArg;
	if (!StrToIntEx(arg, STIF_DEFAULT, &nArg)) {
		nArg = INT_MAX;
	}
	if (!lstrcmpi(cmd, TEXT("help"))) {
		static const TCHAR text[] =
			TEXT("@help\t�w���v��\��")
			TEXT("\n@fopa N\t�������̓��߃��x��1�`10(N���ȗ������10)�B")
			TEXT("\n@fwd N\t�R�����g�̑O�i")
			TEXT("\n@size N\t�R�����g�̕����T�C�Y��N%�ɂ���(N���ȗ������100%)�B")
			TEXT("\n@speed N\t�R�����g�̑��x��N%�ɂ���(N���ȗ������100%)�B")
			TEXT("\n@rl\t�u�����X�g�̂��ׂĂ�Comment�����X�g����")
			TEXT("\n@rr\t�u�����X�g��ݒ�t�@�C������ēǂݍ��݂���")
			TEXT("\n@ra N\tPatternN0�`N9��L���ɂ���")
			TEXT("\n@rm N\tPatternN0�`N9�𖳌��ɂ���")
			TEXT("\n@debug N\t�f�o�b�O0�`15");
		MessageBox(hForce_, text, TEXT("NicoJK - ���[�J���R�}���h"), MB_OK);
	} else if (!lstrcmpi(cmd, TEXT("fopa"))) {
		s_.forceOpacity = 0 < nArg && nArg < 10 ? nArg * 255 / 10 : 255;
		LONG style = GetWindowLong(hForce_, GWL_EXSTYLE);
		SetWindowLong(hForce_, GWL_EXSTYLE, s_.forceOpacity == 255 ? style & ~WS_EX_LAYERED : style | WS_EX_LAYERED);
		SetLayeredWindowAttributes(hForce_, 0, static_cast<BYTE>(s_.forceOpacity), LWA_ALPHA);
	} else if (!lstrcmpi(cmd, TEXT("fwd")) && nArg != INT_MAX) {
		if (nArg == 0) {
			forwardOffsetDelta_ = -forwardOffset_;
		} else {
			forwardOffsetDelta_ += nArg;
		}
	} else if (!lstrcmpi(cmd, TEXT("size"))) {
		int rate = min(max(nArg == INT_MAX ? 100 : nArg, 10), 1000);
		commentWindow_.SetCommentSize(s_.commentSize * rate / 100, s_.commentSizeMin, s_.commentSizeMax, s_.commentLineMargin);
		TCHAR text[64];
		wsprintf(text, TEXT("���݂̃R�����g�̕����T�C�Y��%d%%�ł��B"), rate);
		OutputMessageLog(text);
	} else if (!lstrcmpi(cmd, TEXT("speed"))) {
		commentWindow_.SetDisplayDuration(s_.commentDuration * 100 / (nArg <= 0 || nArg == INT_MAX ? 100 : nArg));
		TCHAR text[64];
		wsprintf(text, TEXT("���݂̃R�����g�̕\�����Ԃ�%dmsec�ł��B"), commentWindow_.GetDisplayDuration());
		OutputMessageLog(text);
	} else if (!lstrcmpi(cmd, TEXT("rl"))) {
		TCHAR text[2048];
		text[0] = TEXT('\0');
		std::vector<RPL_ELEM>::const_iterator it = rplList_.begin();
		for (int len = 0; it != rplList_.end() && len + _countof(it->comment) + 32 < _countof(text); ++it) {
			if (it->comment[0] && !lstrcmpi(it->section, TEXT("CustomReplace"))) {
				len += wsprintf(&text[len], TEXT("%sPattern%d=%s\n"), it->IsEnabled() ? TEXT("") : TEXT("#"), it->key, it->comment);
			}
		}
		MessageBox(hForce_, text, TEXT("NicoJK - ���[�J���R�}���h"), MB_OK);
	} else if (!lstrcmpi(cmd, TEXT("rr"))) {
		rplList_.clear();
		LoadRplListFromIni(TEXT("AutoReplace"), &rplList_);
		LoadRplListFromIni(TEXT("CustomReplace"), &rplList_);
		OutputMessageLog(TEXT("�u�����X�g���ēǂݍ��݂��܂����B"));
	} else if (!lstrcmpi(cmd, TEXT("ra")) || !lstrcmpi(cmd, TEXT("rm"))) {
		bool bFound = false;
		std::vector<RPL_ELEM>::iterator it = rplList_.begin();
		for (; it != rplList_.end(); ++it) {
			if (it->key / 10 == nArg && !lstrcmpi(it->section, TEXT("CustomReplace"))) {
				bFound = true;
				it->SetEnabled(cmd[1] == TEXT('a'));
				TCHAR text[_countof(it->comment) + 64];
				wsprintf(text, TEXT("Pattern%d(%s)��%c���ɂ��܂����B"), it->key, it->comment, it->IsEnabled() ? TEXT('�L') : TEXT('��'));
				OutputMessageLog(text);
			}
		}
		if (bFound) {
			SaveRplListToIni(TEXT("CustomReplace"), rplList_, false);
		} else {
			OutputMessageLog(TEXT("Error:�p�^�[����������܂���B"));
		}
	} else if (!lstrcmpi(cmd, TEXT("debug"))) {
		commentWindow_.SetDebugFlags(nArg);
	} else {
		OutputMessageLog(TEXT("Error:�s���ȃ��[�J���R�}���h�ł��B"));
	}
}

INT_PTR CALLBACK CNicoJK::ForceDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG) {
		SetWindowLongPtr(hwnd, DWLP_USER, lParam);
	}
	return reinterpret_cast<CNicoJK*>(GetWindowLongPtr(hwnd, DWLP_USER))->ForceDialogProcMain(hwnd, uMsg, wParam, lParam);
}

#include "NicoJKKakolog/IniFile.h"

INT_PTR CNicoJK::ForceDialogProcMain(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//---NicoJKKakolog---
	if(uMsg!=WM_INITDIALOG)
		if (nicoJKKakolog.DialogProc(uMsg, wParam, lParam))
			return TRUE;
	//---NicoJKKakolog---

	switch (uMsg) {
	case WM_INITDIALOG:
		{
			logList_.clear();
			logListDisplayedSize_ = 0;
			commentWindow_.SetStyle(s_.commentFontName, s_.commentFontNameMulti, s_.bCommentFontBold, s_.bCommentFontAntiAlias,
			                        s_.commentFontOutline, s_.bUseOsdCompositor, s_.bUseTexture, s_.bUseDrawingThread);
			commentWindow_.SetCommentSize(s_.commentSize, s_.commentSizeMin, s_.commentSizeMax, s_.commentLineMargin);
			commentWindow_.SetDisplayDuration(s_.commentDuration);
			commentWindow_.SetOpacity(static_cast<BYTE>(s_.commentOpacity));
			if (commentWindow_.GetOpacity() != 0 && m_pApp->GetPreview()) {
				HWND hwndContainer = FindVideoContainer();
				commentWindow_.Create(hwndContainer);
				bHalfSkip_ = GetWindowHeight(hwndContainer) >= s_.halfSkipThreshold;
				ProcessChatTag("<!--<chat date=\"0\" mail=\"cyan ue\" user_id=\"-\">(NicoJK ON)</chat>-->");
			}
			bDisplayLogList_ = (s_.hideForceWindow & 2) != 0;
			forwardTick_ = timeGetTime();
			forwardOffset_ = 0;
			forwardOffsetDelta_ = 0;
			currentJKToGet_ = -1;
			jkLeaveThreadID_[0] = '\0';
			jkLeaveThreadCheck_ = 0;
			commentServerResponse_[0] = '\0';
			lastPostComm_[0] = TEXT('\0');
			bUsingLogfileDriver_ = IsMatchDriverName(s_.logfileDrivers);
			readLogfileTick_ = GetTickCount();
			bSpecFile_ = false;
			dropFileTimeout_ = 0;
			SendMessage(hwnd, WM_RESET_STREAM, 0, 0);

			channelSocket_.SetDoHalfClose(s_.bDoHalfClose);
			jkSocket_.SetDoHalfClose(s_.bDoHalfClose);
			postSocket_.SetDoHalfClose(s_.bDoHalfClose);

			SendDlgItemMessage(hwnd, bDisplayLogList_ ? IDC_RADIO_LOG : IDC_RADIO_FORCE, BM_SETCHECK, BST_CHECKED, 0);
			SendDlgItemMessage(hwnd, IDC_CHECK_RELATIVE, BM_SETCHECK, s_.bSetRelative ? BST_CHECKED : BST_UNCHECKED, 0);
			SendDlgItemMessage(hwnd, IDC_SLIDER_OPACITY, TBM_SETRANGE, TRUE, MAKELPARAM(0, 10));
			SendDlgItemMessage(hwnd, IDC_SLIDER_OPACITY, TBM_SETPOS, TRUE, (commentWindow_.GetOpacity() * 10 + 254) / 255);
			SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_CB_POST, CBN_EDITCHANGE), 0);
			SetTimer(hwnd, TIMER_UPDATE, max(UPDATE_FORCE_INTERVAL, 10000), NULL);
			if (s_.timerInterval >= 0) {
				SetTimer(hwnd, TIMER_FORWARD, s_.timerInterval, NULL);
			}
			SetTimer(hwnd, TIMER_SETUP_CURJK, SETUP_CURJK_DELAY, NULL);
			PostMessage(hwnd, WM_TIMER, TIMER_UPDATE, 0);
			PostMessage(hwnd, WM_TIMER, TIMER_JK_WATCHDOG, 0);
			// �ʒu�𕜌�
			HMONITOR hMon = MonitorFromRect(&s_.rcForce, MONITOR_DEFAULTTONEAREST);
			MONITORINFO mi;
			mi.cbSize = sizeof(MONITORINFO);
			if (s_.rcForce.right <= s_.rcForce.left || !GetMonitorInfo(hMon, &mi) ||
			    s_.rcForce.right < mi.rcMonitor.left + 20 || mi.rcMonitor.right - 20 < s_.rcForce.left ||
			    s_.rcForce.bottom < mi.rcMonitor.top + 20 || mi.rcMonitor.bottom - 20 < s_.rcForce.top) {
				GetWindowRect(hwnd, &s_.rcForce);
			}
			MoveWindow(hwnd, 0, 0, 64, 64, FALSE);
			MoveWindow(hwnd, s_.rcForce.left, s_.rcForce.top, s_.rcForce.right - s_.rcForce.left, s_.rcForce.bottom - s_.rcForce.top, FALSE);
			// �s�����x�𕜌�
			LONG style = GetWindowLong(hwnd, GWL_EXSTYLE);
			SetWindowLong(hwnd, GWL_EXSTYLE, s_.forceOpacity == 255 ? style & ~WS_EX_LAYERED : style | WS_EX_LAYERED);
			SetLayeredWindowAttributes(hwnd, 0, static_cast<BYTE>(s_.forceOpacity), LWA_ALPHA);

			if ((s_.hideForceWindow & 1) == 0) {
				ShowWindow(hwnd, SW_SHOWNA);
				SendMessage(hwnd, WM_SET_ZORDER, 0, 0);
			}
			// TVTest�N�������Video Container�E�B���h�E�̔z�u����܂��Ă��Ȃ��悤�Ȃ̂ōēx������
			SetTimer(hwnd, TIMER_DONE_SIZE, 500, NULL);


			//---NicoJKKakoLog---
			nicoJKKakolog.DialogInit(hwnd, GetDlgItem(hwnd, IDC_LISTVIEW));
			//---NicoJKKakoLog---
		}
		return TRUE;
	case WM_DESTROY:
		{
			// �ʒu��ۑ�
			GetWindowRect(hwnd, &s_.rcForce);
			s_.commentOpacity = (s_.commentOpacity&~0xFF) | commentWindow_.GetOpacity();
			s_.bSetRelative = SendDlgItemMessage(hwnd, IDC_CHECK_RELATIVE, BM_GETCHECK, 0, 0) == BST_CHECKED;
			// ���O�t�@�C�������
			WriteToLogfile(-1);
			ReadFromLogfile(-1);
			if (bSpecFile_) {
				DeleteFile(tmpSpecFileName_);
			}
			commentWindow_.Destroy();
			channelSocket_.Close();
			jkSocket_.Close();
			postSocket_.Close();
		}
		return FALSE;
	case WM_DROPFILES:
		dropFileTimeout_ = 0;
		if (DragQueryFile(reinterpret_cast<HDROP>(wParam), 0, dropFileName_, _countof(dropFileName_))) {
			if (bSpecFile_) {
				ReadFromLogfile(-1);
				DeleteFile(tmpSpecFileName_);
				bSpecFile_ = false;
			}
			SendDlgItemMessage(hwnd, IDC_CHECK_SPECFILE, BM_SETCHECK, BST_UNCHECKED, 0);
			dropFileTimeout_ = 1;
			SetTimer(hwnd, TIMER_OPEN_DROPFILE, 0, NULL);
		}
		break;
	case WM_HSCROLL:
		if (reinterpret_cast<HWND>(lParam) == GetDlgItem(hwnd, IDC_SLIDER_OPACITY) && LOWORD(wParam) == SB_THUMBTRACK) {
			BYTE newOpacity = static_cast<BYTE>(HIWORD(wParam) * 255 / 10);
			if (commentWindow_.GetOpacity() == 0 && newOpacity != 0 && m_pApp->GetPreview()) {
				commentWindow_.ClearChat();
				HWND hwnd = FindVideoContainer();
				commentWindow_.Create(hwnd);
				bHalfSkip_ = GetWindowHeight(hwnd) >= s_.halfSkipThreshold;
			} else if (commentWindow_.GetOpacity() != 0 && newOpacity == 0) {
				commentWindow_.Destroy();
			}
			commentWindow_.SetOpacity(newOpacity);
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_RADIO_FORCE:
		case IDC_RADIO_LOG:
			//---NicoJKKakolog---
			nicoJKKakolog.showingListView = false;
			ShowWindow(GetDlgItem(hwnd,IDC_LISTVIEW), SW_HIDE);
			ShowWindow(GetDlgItem(hwnd,IDC_FORCELIST), SW_SHOW);
			//---NicoJKKakolog---

			bDisplayLogList_ = SendDlgItemMessage(hwnd, IDC_RADIO_LOG, BM_GETCHECK, 0, 0 ) == BST_CHECKED;
			SendMessage(hwnd, WM_UPDATE_LIST, TRUE, 0);
			PostMessage(hwnd, WM_TIMER, TIMER_UPDATE, 0);
			break;
		//---NicoJKKakolog---
		case IDC_RADIO_CHATSELECT:
			nicoJKKakolog.showingListView = true;
			ShowWindow(GetDlgItem(hwnd, IDC_LISTVIEW), SW_SHOW);
			ShowWindow(GetDlgItem(hwnd, IDC_FORCELIST), SW_HIDE);
			break;
			//---NicoJKKakolog---
		case IDC_CHECK_SPECFILE:
			if (bSpecFile_ != (SendDlgItemMessage(hwnd, IDC_CHECK_SPECFILE, BM_GETCHECK, 0, 0) == BST_CHECKED)) {
				if (bSpecFile_) {
					ReadFromLogfile(-1);
					DeleteFile(tmpSpecFileName_);
					bSpecFile_ = false;
				} else {
					FILETIME ft;
					TCHAR path[MAX_PATH];
					bool bRel = SendDlgItemMessage(hwnd, IDC_CHECK_RELATIVE, BM_GETCHECK, 0, 0) == BST_CHECKED;
					// �_�C�A���O���J���Ă���Ԃ�D&D����邩������Ȃ�
					if ((!bRel || GetCurrentTot(&ft)) &&
					    FileOpenDialog(hwnd, TEXT("�������O(*.jkl;*.xml)\0*.jkl;*.xml\0���ׂẴt�@�C��\0*.*\0"), path, _countof(path)) &&
					    !bSpecFile_ && ImportLogfile(path, tmpSpecFileName_, bRel ? FileTimeToUnixTime(ft) + 2 : 0))
					{
						readLogfileTick_ = GetTickCount();
						bSpecFile_ = true;
					}
				}
				SendDlgItemMessage(hwnd, IDC_CHECK_SPECFILE, BM_SETCHECK, bSpecFile_ ? BST_CHECKED : BST_UNCHECKED, 0);
			}
			break;
		case IDC_FORCELIST:
			if (HIWORD(wParam) == LBN_SELCHANGE) {
				if (!bDisplayLogList_) {
					// �������X�g�\����
					int index = ListBox_GetCurSel((HWND)lParam);
					int jkID = -1;
					if (0 <= index && index < (int)forceList_.size()) {
						jkID = forceList_[index].jkID;
					}
					if (currentJKToGet_ != jkID) {
						currentJKToGet_ = jkID;
						jkSocket_.Shutdown();
						commentWindow_.ClearChat();
						SetTimer(hwnd, TIMER_JK_WATCHDOG, 1000, NULL);
					}
				}
			} else if (HIWORD(wParam) == LBN_DBLCLK) {
				int index = ListBox_GetCurSel((HWND)lParam);
				if (bDisplayLogList_ && 0 <= index && index < (int)logList_.size()) {
					std::list<LOG_ELEM>::const_iterator it = logList_.begin();
					for (int i = 0; i < index; ++i, ++it);
					if (it->marker[0] != TEXT('#') && it->marker[0] != TEXT('.')) {
						// ���[�U�[NG�̒u���p�^�[��������
						RPL_ELEM e;
						lstrcpyn(e.section, TEXT("AutoReplace"), _countof(e.section));
						wsprintf(e.pattern, TEXT("s/^<chat(?=.*? user_id=\"%.14s%s.*>.*<)/<chat abone=\"1\"/g"),
						         it->marker, lstrlen(it->marker) > 14 ? TEXT("") : TEXT("\""));
						if (e.AssignFromPattern()) {
							// �����p�^�[�����ǂ������ׂ�
							std::vector<RPL_ELEM> autoRplList;
							LoadRplListFromIni(TEXT("AutoReplace"), &autoRplList);
							std::vector<RPL_ELEM>::const_iterator jt = autoRplList.begin();
							for (; jt != autoRplList.end() && lstrcmp(jt->pattern, e.pattern); ++jt);
							// ���b�Z�[�W�{�b�N�X�Ŋm�F
							TCHAR text[_countof(it->marker) + _countof(it->text) + 32];
							wsprintf(text, TEXT(">>%d ID:%s\n%s"), it->no, it->marker, it->text);
							if (jt != autoRplList.end()) {
								if (MessageBox(hwnd, text, TEXT("NicoJK - NG�y�����z���܂�"), MB_OKCANCEL) == IDOK) {
									autoRplList.erase(jt);
									for (int i = 0; i < (int)autoRplList.size(); autoRplList[i].key = i, ++i);
									SaveRplListToIni(TEXT("AutoReplace"), autoRplList);
								}
							} else {
								if (MessageBox(hwnd, text, TEXT("NicoJK - NG�o�^���܂�"), MB_OKCANCEL) == IDOK) {
									autoRplList.push_back(e);
									while ((int)autoRplList.size() > max(s_.maxAutoReplace, 0)) {
										autoRplList.erase(autoRplList.begin());
									}
									for (int i = 0; i < (int)autoRplList.size(); autoRplList[i].key = i, ++i);
									SaveRplListToIni(TEXT("AutoReplace"), autoRplList);
								}
							}
							// �u�����X�g���X�V
							rplList_ = autoRplList;
							LoadRplListFromIni(TEXT("CustomReplace"), &rplList_);
						}
					}
				}
			}
			break;
		case IDC_CB_POST:
			if (HIWORD(wParam) == CBN_EDITCHANGE) {
				// �R�����g��������쐬����
				TCHAR comm[POST_COMMENT_MAX + 32];
				GetPostComboBoxText(comm, _countof(comm));
				while (SendDlgItemMessage(hwnd, IDC_CB_POST, CB_DELETESTRING, 0, 0) > 0);
				for (LPCTSTR p = s_.mailDecorations; *p; ) {
					int len = StrCSpn(p, TEXT(":"));
					TCHAR text[_countof(comm) + 64];
					lstrcpyn(text, p, min(len + 1, 64));
					lstrcat(text, comm);
					SendDlgItemMessage(hwnd, IDC_CB_POST, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(text));
					p += p[len] ? len + 1 : len;
				}
				// �������x������
				int excess = lstrlen(comm) - (POST_COMMENT_MAX - 1);
				if (excess > 0) {
					TCHAR text[64];
					wsprintf(text, TEXT("Warning:%d�����𒴂��Ă��܂�(+%d)�B"), POST_COMMENT_MAX - 1, excess);
					OutputMessageLog(text);
				}
			}
			break;
		case IDOK:
		case IDCANCEL:
			// �B������
			ShowWindow(hwnd, SW_HIDE);
			SetWindowLongPtr(hwnd, DWLP_MSGRESULT, 0);
			return TRUE;
		}
		break;
	case WM_TIMER:
		switch (wParam) {
		case TIMER_UPDATE:
			if (!bDisplayLogList_ && IsWindowVisible(hwnd)) {
				// �������X�V����
				char szGet[_countof(cookie_) + 256];
				lstrcpyA(szGet, "GET /api/v2_app/getchannels HTTP/1.1\r\n");
				AppendHttpHeader(szGet, "Host: ", JK_HOST_NAME, "\r\n");
				AppendHttpHeader(szGet, "Cookie: ", cookie_, "\r\n");
				AppendHttpHeader(szGet, "Connection: ", "close", "\r\n\r\n");
				if (channelSocket_.Send(hwnd, WMS_FORCE, JK_HOST_NAME, 80, szGet)) {
					channelBuf_.clear();
				}
			}
			break;
		case TIMER_JK_WATCHDOG:
			SetTimer(hwnd, TIMER_JK_WATCHDOG, max(JK_WATCHDOG_INTERVAL, 10000), NULL);
			if (jkLeaveThreadCheck_ > 0 && --jkLeaveThreadCheck_ == 0) {
				OutputMessageLog(TEXT("leave_thread�^�O�ɂ��ؒf���܂��B"));
				jkSocket_.Shutdown();
				SetTimer(hwnd, TIMER_JK_WATCHDOG, 1000, NULL);
			}
			if (currentJKToGet_ >= 0 && !bUsingLogfileDriver_) {
				// �p�[�}�����N���擾
				char szGet[_countof(cookie_) + 256];
				wsprintfA(szGet, "GET /api/v2/getflv?v=jk%d HTTP/1.1\r\n", currentJKToGet_);
				AppendHttpHeader(szGet, "Host: ", JK_HOST_NAME, "\r\n");
				AppendHttpHeader(szGet, "Cookie: ", cookie_, "\r\n");
				AppendHttpHeader(szGet, "Connection: ", "close", "\r\n\r\n");
				if (jkSocket_.Send(hwnd, WMS_JK, JK_HOST_NAME, 80, szGet)) {
					currentJK_ = currentJKToGet_;
					bConnectedToCommentServer_ = false;
					jkBuf_.clear();
					OutputMessageLog(TEXT("�p�[�}�����N�ɐڑ��J�n���܂����B"));
				}
			}
			break;
		case TIMER_FORWARD:
			bFlipFlop_ = !bFlipFlop_;
			if (hSyncThread_ || !bHalfSkip_ || bFlipFlop_) {
				// �I�t�Z�b�g�𒲐�����
				bool bNotify = false;
				if (0 < forwardOffsetDelta_ && forwardOffsetDelta_ <= 30000) {
					// �O�i�����Ē���
					int delta = min(forwardOffsetDelta_, forwardOffsetDelta_ < 10000 ? 500 : 2000);
					forwardOffset_ += delta;
					forwardOffsetDelta_ -= delta;
					bNotify = forwardOffsetDelta_ == 0;
					commentWindow_.Forward(delta);
				} else if (forwardOffsetDelta_ != 0) {
					// ���O�t�@�C������Ĉ�C�ɒ���
					forwardOffset_ += forwardOffsetDelta_;
					forwardOffsetDelta_ = 0;
					bNotify = true;
					ReadFromLogfile(-1);
					commentWindow_.ClearChat();
				}
				if (bNotify) {
					TCHAR text[32];
					wsprintf(text, TEXT("(Offset %d)"), forwardOffset_ / 1000);
					commentWindow_.AddChat(text, RGB(0x00,0xFF,0xFF), CCommentWindow::CHAT_POS_UE);
				}
				// �R�����g�̕\����i�߂�
				DWORD tick = timeGetTime();
				commentWindow_.Forward(min(static_cast<int>(tick - forwardTick_), 5000));
				forwardTick_ = tick;

				// �`���b�g���Ƃ��Ă��ĕ\��
				FILETIME ft;
				if (GetCurrentTot(&ft)) {
					bool bRead = false;
					//char text[CHAT_TAG_MAX];
					unsigned int tm = FileTimeToUnixTime(ft);
					tm = forwardOffset_ < 0 ? tm - (-forwardOffset_ / 1000) : tm + forwardOffset_ / 1000;

					//---NicoJKKakoLog---

					/*while (ReadFromLogfile(bSpecFile_ ? 0 : currentJKToGet_, text, _countof(text), tm)) {
						ProcessChatTag(text);
						bRead = true;
					}*/
					
					for (const NicoJKKakolog::Chat &chat : nicoJKKakolog.GetChats(tm))
					{
						commentWindow_.AddChat(NicoJKKakolog::NicoJKKakolog::utf8_wide_conv.from_bytes(chat.text).c_str(), chat.color,
							(chat.position == NicoJKKakolog::Chat::Position::Default) ? CCommentWindow::CHAT_POS_DEFAULT : (chat.position == NicoJKKakolog::Chat::Position::Down) ? CCommentWindow::CHAT_POS_SHITA : CCommentWindow::CHAT_POS_UE,
							(chat.size == NicoJKKakolog::Chat::Size::Default) ? CCommentWindow::CHAT_SIZE_DEFAULT : CCommentWindow::CHAT_SIZE_SMALL);

						bRead = true;
					}
					
					//---NicoJKKakoLog---

					if (bRead) {
						// date�����l�͕b���x�����Ȃ��̂ŃR�����g�\�����c�q�ɂȂ�Ȃ��悤�K���ɂ��܂���
						commentWindow_.ScatterLatestChats(1000);
						PostMessage(hwnd, WM_UPDATE_LIST, FALSE, 0);
					}
				}
				commentWindow_.Update();
				bPendingTimerForward_ = false;
			}
			break;
		case TIMER_SETUP_CURJK:
			{
				// ������Ԃ��ω������̂Ŏ������̃T�[�r�X�ɑΉ��������ID�𒲂ׂĕύX����
				KillTimer(hwnd, TIMER_SETUP_CURJK);
				NETWORK_SERVICE_ID_ELEM e = {GetCurrentNetworkServiceID(), 0};
				std::vector<NETWORK_SERVICE_ID_ELEM>::const_iterator it =
					std::lower_bound(ntsIDList_.begin(), ntsIDList_.end(), e, NETWORK_SERVICE_ID_ELEM::COMPARE());
				int jkID = it!=ntsIDList_.end() && (it->ntsID==e.ntsID || !(e.ntsID&0xFFFF) && e.ntsID==(it->ntsID&0xFFFF0000)) ? it->jkID : -1;
				if (currentJKToGet_ != jkID) {
					currentJKToGet_ = jkID;
					jkSocket_.Shutdown();
					//commentWindow_.ClearChat();
					SetTimer(hwnd, TIMER_JK_WATCHDOG, 1000, NULL);
					// �I�����ڂ��X�V���邽��
					SendMessage(hwnd, WM_UPDATE_LIST, TRUE, 0);
				}
			}
			break;
		case TIMER_OPEN_DROPFILE:
			// D&D���ꂽ�������O�t�@�C�����J��
			// TS�t�@�C���Ƃ̓���D&D���l������Rel�`�F�b�N���͊�Ƃ���TOT�̎擾�^�C�~���O��x�点��
			if (--dropFileTimeout_ < 0 || bSpecFile_) {
				KillTimer(hwnd, TIMER_OPEN_DROPFILE);
			} else {
				FILETIME ft;
				bool bRel = SendDlgItemMessage(hwnd, IDC_CHECK_RELATIVE, BM_GETCHECK, 0, 0) == BST_CHECKED;
				if (!bRel || GetCurrentTot(&ft)) {
					KillTimer(hwnd, TIMER_OPEN_DROPFILE);
					if (ImportLogfile(dropFileName_, tmpSpecFileName_, bRel ? FileTimeToUnixTime(ft) + 2 : 0)) {
						readLogfileTick_ = GetTickCount();
						bSpecFile_ = true;
						SendDlgItemMessage(hwnd, IDC_CHECK_SPECFILE, BM_SETCHECK, BST_CHECKED, 0);
					}
				}
			}
			break;
		case TIMER_DONE_MOVE:
			KillTimer(hwnd, TIMER_DONE_MOVE);
			commentWindow_.OnParentMove();
			break;
		case TIMER_DONE_SIZE:
			KillTimer(hwnd, TIMER_DONE_SIZE);
			commentWindow_.OnParentSize();
			bHalfSkip_ = GetWindowHeight(FindVideoContainer()) >= s_.halfSkipThreshold;
			break;
		case TIMER_DONE_POSCHANGE:
			KillTimer(hwnd, TIMER_DONE_POSCHANGE);
			if (!m_pApp->GetFullscreen() && ((s_.hideForceWindow & 4) || (GetWindowLong(m_pApp->GetAppWindow(), GWL_STYLE) & WS_MAXIMIZE))) {
				SendMessage(hwnd, WM_SET_ZORDER, 0, 0);
			}
			break;
		}
		break;
	case WM_RESET_STREAM:
		dprintf(TEXT("CNicoJK::ForceDialogProcMain() WM_RESET_STREAM\n")); // DEBUG
		{
			CBlockLock lock(&streamLock_);
			ftTot_[0].dwHighDateTime = 0xFFFFFFFF;
		}
		ReadFromLogfile(-1);
		return TRUE;
	case WM_UPDATE_LIST:
		{
			HWND hList = GetDlgItem(hwnd, IDC_FORCELIST);
			if (!bDisplayLogList_ || !IsWindowVisible(hwnd)) {
				// ���X�g�����������Ȃ��悤�ɂ���
				for (; logList_.size() > COMMENT_TRIMEND; logList_.pop_front());
				logListDisplayedSize_ = 0;
			}
			if (!IsWindowVisible(hwnd)) {
				// ��\�����̓T�{��
				if (ListBox_GetCount(hList) != 0) {
					ListBox_ResetContent(hList);
				}
				return TRUE;
			} else if (!bDisplayLogList_ && !wParam) {
				// �������X�g�\�����͍����X�V(wParam==FALSE)���Ȃ�
				return TRUE;
			}
			// �`����ꎞ��~
			SendMessage(hList, WM_SETREDRAW, FALSE, 0);
			int iTopItemIndex = ListBox_GetTopIndex(hList);
			// wParam!=FALSE�̂Ƃ��̓��X�g�̓��e�����Z�b�g����
			if (wParam) {
				ListBox_ResetContent(hList);
				// wParam==2�̂Ƃ��̓X�N���[���ʒu��ۑ�����
				if (wParam != 2) {
					iTopItemIndex = 0;
				}
			}
			if (bDisplayLogList_) {
				// ���O���X�g�\����
				int iSelItemIndex = ListBox_GetCurSel(hList);
				if (logList_.size() < logListDisplayedSize_ || ListBox_GetCount(hList) != logListDisplayedSize_) {
					ListBox_ResetContent(hList);
					logListDisplayedSize_ = 0;
				}
				// logList_�ƃ��X�g�{�b�N�X�̓��e����ɓ�������悤�ɍX�V����
				std::list<LOG_ELEM>::const_iterator it = logList_.end();
				for (size_t i = logList_.size() - logListDisplayedSize_; i > 0; --i, --it);
				for (; it != logList_.end(); ++it) {
					TCHAR text[_countof(it->text) + 64];
					wsprintf(text, TEXT("%02d:%02d:%02d (%.3s) %s"), it->st.wHour, it->st.wMinute, it->st.wSecond, it->marker, it->text);
					ListBox_AddString(hList, text);
					++logListDisplayedSize_;
				}
				while (logList_.size() > COMMENT_TRIMEND) {
					logList_.pop_front();
					ListBox_DeleteString(hList, 0);
					--logListDisplayedSize_;
					--iSelItemIndex;
					--iTopItemIndex;
				}
				if (iSelItemIndex < 0) {
					ListBox_SetTopIndex(hList, ListBox_GetCount(hList) - 1);
				} else {
					ListBox_SetCurSel(hList, iSelItemIndex);
					ListBox_SetTopIndex(hList, max(iTopItemIndex, 0));
				}
			} else {
				// �������X�g�\����
				std::vector<FORCE_ELEM>::const_iterator it = forceList_.begin();
				for (; it != forceList_.end(); ++it) {
					TCHAR text[_countof(it->name) + 64];
					wsprintf(text, TEXT("jk%d (%s) �����F%d"), it->jkID, it->name, it->force);
					ListBox_AddString(hList, text);
					if (it->jkID == currentJKToGet_) {
						ListBox_SetCurSel(hList, ListBox_GetCount(hList) - 1);
					}
				}
				ListBox_SetTopIndex(hList, iTopItemIndex);
			}
			// �`����ĊJ
			SendMessage(hList, WM_SETREDRAW, TRUE, 0);
			InvalidateRect(hList, NULL, FALSE);
		}
		return TRUE;
	case WMS_FORCE:
		{
			static const std::regex reChannel("<((?:bs_)?channel)>([^]*?)</\\1>");
			static const std::regex reChannelRadio("<((?:bs_|radio_)?channel)>([^]*?)</\\1>");
			static const std::regex reVideo("<video>jk(\\d+)</video>");
			static const std::regex reForce("<force>(\\d+)</force>");
			static const std::regex reName("<name>([^<]*)</name>");

			int ret = channelSocket_.ProcessRecv(wParam, lParam, &channelBuf_);
			if (ret == -2) {
				// �ؒf
				channelBuf_.push_back('\0');
				bool bCleared = false;
				std::cmatch m, mVideo, mForce, mName;
				const char *p = &channelBuf_[FindHttpBody(&channelBuf_[0])];
				const char *pLast = &p[lstrlenA(p)];
				for (; std::regex_search(p, pLast, m, s_.bShowRadio ? reChannelRadio : reChannel); p = m[0].second) {
					if (std::regex_search(m[2].first, m[2].second, mVideo, reVideo) &&
					    std::regex_search(m[2].first, m[2].second, mForce, reForce) &&
					    std::regex_search(m[2].first, m[2].second, mName, reName))
					{
						FORCE_ELEM e;
						e.jkID = atoi(mVideo[1].first);
						e.force = atoi(mForce[1].first);
						int len = MultiByteToWideChar(CP_UTF8, 0, mName[1].first, static_cast<int>(mName[1].length()), e.name, _countof(e.name) - 1);
						e.name[len] = TEXT('\0');
						DecodeEntityReference(e.name);
						if (!bCleared) {
							forceList_.clear();
							bCleared = true;
						}
						forceList_.push_back(e);
					}
				}
				SendMessage(hwnd, WM_UPDATE_LIST, 2, 0);
			}
		}
		return TRUE;
	case WMS_JK:
		{
			static const std::regex reMs("^(?=.*?(?:^|&)done=true(?:&|$)).*?(?:^|&)ms=(\\d+\\.\\d+\\.\\d+\\.\\d+)(?:&|$)");
			static const std::regex reMsPort("(?:^|&)ms_port=(\\d+)(?:&|$)");
			static const std::regex reThreadID("(?:^|&)thread_id=(\\d+)(?:&|$)");
			static const std::regex reUserID("(?:^|&)user_id=(\\d+)(?:&|$)");
			static const std::regex reIsPremium("(?:^|&)is_premium=1(?:&|$)");
			static const std::regex reNickname("(?:^|&)nickname=([0-9A-Za-z]*)");
			static const std::regex reChatNo("^<chat(?= )[^>]*? no=\"(\\d+)\"");
			static const std::regex reChatResult("^<chat_result(?= ).*? status=\"(?!0\")(\\d+)\"");
			static const std::regex reLeaveThreadID("^<leave_thread(?= )(?=.*? reason=\"2\").*? thread=\"(\\d+)\"");

			int ret = jkSocket_.ProcessRecv(wParam, lParam, &jkBuf_);
			if (ret < 0) {
				// �ؒf
				if (bConnectedToCommentServer_) {
					bConnectedToCommentServer_ = false;
					commentServerResponse_[0] = '\0';
					jkLeaveThreadCheck_ = 0;
					OutputMessageLog(TEXT("�R�����g�T�[�o�Ƃ̒ʐM��ؒf���܂����B"));
					WriteToLogfile(-1);
				} else if (ret == -2 && currentJK_ == currentJKToGet_) {
					jkBuf_.push_back('\0');
					const char *p = &jkBuf_[FindHttpBody(&jkBuf_[0])];
					std::cmatch mMs, mMsPort, mThreadID;
					if (std::regex_search(p, mMs, reMs) &&
					    std::regex_search(p, mMsPort, reMsPort) &&
					    std::regex_search(p, mThreadID, reThreadID) &&
					    lstrcmpA(jkLeaveThreadID_, mThreadID[1].str().c_str()))
					{
						// �R�����g�T�[�o�ɐڑ�
						static const char szRequestTemplate[] = "<thread res_from=\"-10\" version=\"20061206\" thread=\"%.15s\" />";
						char szRequest[_countof(szRequestTemplate) + 16];
						wsprintfA(szRequest, szRequestTemplate, mThreadID[1].str().c_str());
						jkLeaveThreadID_[0] = '\0';
						// '\0'�܂ő���
						if (jkSocket_.Send(hwnd, WMS_JK, mMs[1].str().c_str(), static_cast<unsigned short>(atoi(mMsPort[1].first)), szRequest, lstrlenA(szRequest) + 1, true)) {
							bConnectedToCommentServer_ = true;
							jkBuf_.clear();
							// �R�����g���e�̂���
							bGetflvIsPremium_ = std::regex_search(p, reIsPremium);
							getflvUserID_[0] = '\0';
							std::cmatch m;
							if (std::regex_search(p, m, reUserID)) {
								lstrcpynA(getflvUserID_, m[1].str().c_str(), _countof(getflvUserID_));
							}
							if (getflvUserID_[0] && std::regex_search(p, m, reNickname)) {
								TCHAR text[128];
								wsprintf(text, TEXT("�R�����g�T�[�o�ɐڑ��J�n���܂���(login=%.16S)�B"), m[1].str().c_str());
								OutputMessageLog(text);
							} else {
								OutputMessageLog(TEXT("�R�����g�T�[�o�ɐڑ��J�n���܂����B"));
							}
						}
					}
				}
			} else {
				// ��M��
				if (bConnectedToCommentServer_) {
					jkBuf_.push_back('\0');
					const char *p = &jkBuf_[0];
					const char *tail = &jkBuf_[jkBuf_.size() - 1];
					bool bRead = false;
					bool bCarried = false;
					while (p < tail) {
						// ���O�œ��ʂȈӖ��������߉��s�����͐��l�����Q�Ƃɒu��
						int len, rplLen;
						char rpl[CHAT_TAG_MAX + 16];
						for (len = 0, rplLen = 0; p[len]; ++len) {
							if (rplLen < CHAT_TAG_MAX) {
								if (p[len] == '\n' || p[len] == '\r') {
									rplLen += wsprintfA(&rpl[rplLen], "&#%d;", p[len]);
								} else {
									rpl[rplLen++] = p[len];
								}
							}
						}
						rpl[rplLen] = '\0';

						if (&p[len] == tail) {
							// �^�O�̓r���Ńp�P�b�g���������ꍇ�����邽�ߌJ��z��
							jkBuf_.pop_back();
							jkBuf_.erase(jkBuf_.begin(), jkBuf_.end() - len);
							bCarried = true;
							break;
						}
						// �w��t�@�C���Đ����͍�����ƟT�������̂ŕ\�����Ȃ��B��ގw��͂�����x���f
						if (ProcessChatTag(rpl, !bSpecFile_, min(max(-forwardOffset_, 0), 30000))) {
							dprintf(TEXT("#")); // DEBUG
							WriteToLogfile(currentJK_, rpl);
							bRead = true;
						}
						std::cmatch m;
						if (std::regex_search(rpl, m, reChatNo)) {
							// �R�����g���e�̂��߂ɍŐV�R�����g�̃R���Ԃ��L�^
							lastChatNo_ = atoi(m[1].first);
						} else if (!commentServerResponse_[0] && !StrCmpNA(rpl, "<thread ", 8)) {
							// �R�����g���e�̂��߂ɐڑ��������L�^�B���ꂪ�󕶎���łȂ��Ԃ͓��e�\
							lstrcpynA(commentServerResponse_, rpl, _countof(commentServerResponse_));
							commentServerResponseTick_ = GetTickCount();
						} else if (std::regex_search(rpl, m, reChatResult)) {
							// �R�����g���e���s�̉������擾����
							TCHAR text[64];
							wsprintf(text, TEXT("Error:�R�����g���e�Ɏ��s���܂���(status=%d)�B"), atoi(m[1].first));
							OutputMessageLog(text);
						} else if (std::regex_search(rpl, m, reLeaveThreadID)) {
							// leave_thread reason="2"(��4�����Z�b�g?)�ɂ��ؒf����悤�Ƃ��Ă���
							lstrcpynA(jkLeaveThreadID_, m[1].str().c_str(), _countof(jkLeaveThreadID_));
							// ���܂ɃT�[�o����ؒf����Ȃ��ꍇ�����邽��
							jkLeaveThreadCheck_ = 2;
						}
#ifdef _DEBUG
						TCHAR debug[512];
						int debugLen = MultiByteToWideChar(CP_UTF8, 0, p, -1, debug, _countof(debug) - 1);
						debug[debugLen] = TEXT('\0');
						dprintf(TEXT("%s\n"), debug); // DEBUG
#endif
						p += len + 1;
					}
					if (!bCarried) {
						jkBuf_.clear();
					}
					if (bRead && bDisplayLogList_) {
						SendMessage(hwnd, WM_UPDATE_LIST, FALSE, 0);
					}
				}
			}
		}
		return TRUE;
	case WMS_POST:
		{
			static const std::regex rePostkey("postkey=([0-9A-Za-z\\-_]+)");
			static const std::regex reThread("^<thread[^>]*? thread=\"(\\d+)\"");
			static const std::regex reTicket("^<thread[^>]*? ticket=\"(.+?)\"");
			static const std::regex reServerTime("^<thread[^>]*? server_time=\"(\\d+)\"");
			static const std::regex reMailIsValid("[0-9A-Za-z #]*");

			int ret = postSocket_.ProcessRecv(wParam, lParam, &postBuf_);
			if (ret == -2) {
				// �ؒf
				postBuf_.push_back('\0');
				std::cmatch mPostkey, mThread, mTicket, mServerTime;
				const char *p = &postBuf_[FindHttpBody(&postBuf_[0])];
				if (std::regex_search(p, mPostkey, rePostkey) &&
				    std::regex_search(commentServerResponse_, mThread, reThread) &&
				    std::regex_search(commentServerResponse_, mTicket, reTicket) &&
				    std::regex_search(commentServerResponse_, mServerTime, reServerTime) &&
				    getflvUserID_[0] && GetTickCount() - lastPostTick_ >= POST_COMMENT_INTERVAL)
				{
					// �R�����g���𕶎��R�[�h�ϊ�
					TCHAR comm[POST_COMMENT_MAX], mail[64];
					GetPostComboBoxText(comm, _countof(comm), mail, _countof(mail));
					// Tab����or���R�[�h�Z�p���[�^->���s
					for (LPTSTR q = comm; *q; ++q) {
						if (*q == TEXT('\t') || *q == TEXT('\x1e')) *q = TEXT('\n');
					}
					char u8comm[_countof(comm) * 3];
					char u8mail[_countof(mail) * 3];
					char u8commEnc[_countof(comm) * 5];
					int len = WideCharToMultiByte(CP_UTF8, 0, comm, -1, u8comm, _countof(u8comm) - 1, NULL, NULL);
					u8comm[len] = '\0';
					len = WideCharToMultiByte(CP_UTF8, 0, mail, -1, u8mail, _countof(u8mail) - 1, NULL, NULL);
					u8mail[len] = '\0';
					EncodeEntityReference(u8comm, u8commEnc, _countof(u8commEnc));
					// vpos��10msec�P�ʁB�������v�̂���ɉe������Ȃ��悤�ɃT�[�o��������ɕ␳
					int vpos = (int)((LONGLONG)strtoul(mServerTime[1].first, NULL, 10) - strtoul(mThread[1].first, NULL, 10)) * 100 +
					           (int)(GetTickCount() - commentServerResponseTick_) / 10;
					if (std::regex_match(u8mail, reMailIsValid) && vpos >= 0) {
						// �R�����g���e
						static const char szRequestTemplate[] =
							"<chat thread=\"%.15s\" ticket=\"%.15s\" vpos=\"%d\" postkey=\"%.40s\" mail=\"%s%s\" user_id=\"%s\" premium=\"%d\" staff=\"0\">%s</chat>";
						char szRequest[_countof(szRequestTemplate) + _countof(u8commEnc) + _countof(u8mail) + _countof(getflvUserID_) + 256];
						wsprintfA(szRequest, szRequestTemplate, mThread[1].str().c_str(), mTicket[1].str().c_str(), vpos, mPostkey[1].str().c_str(),
						          u8mail, s_.bAnonymity ? " 184" : "", getflvUserID_, (int)bGetflvIsPremium_, u8commEnc);
						// '\0'�܂ő���
						if (jkSocket_.Send(hwnd, WMS_JK, NULL, 0, szRequest, lstrlenA(szRequest) + 1, true)) {
							lastPostTick_ = GetTickCount();
							GetPostComboBoxText(lastPostComm_, _countof(lastPostComm_));
							// �A���h�D�ł���悤�ɑI���폜�ŏ���
							if (SendDlgItemMessage(hwnd, IDC_CB_POST, CB_SETEDITSEL, 0, MAKELPARAM(0, -1)) == TRUE) {
								SendDlgItemMessage(hwnd, IDC_CB_POST, WM_CLEAR, 0, 0);
							}
#ifdef _DEBUG
							OutputDebugStringA((std::string("##POST##") + szRequest + "\n").c_str());
#endif
							return TRUE;
						}
					}
				}
				OutputMessageLog(TEXT("Error:�R�����g���e���L�����Z�����܂����B"));
			}
		}
		return TRUE;
	case WM_SET_ZORDER:
		// �S��ʂ�ő剻���͑O�ʂ̂ق����s�����悢�͂�
		if ((s_.hideForceWindow & 4) || m_pApp->GetFullscreen() || (GetWindowLong(m_pApp->GetAppWindow(), GWL_STYLE) & WS_MAXIMIZE)) {
			// TVTest�E�B���h�E�̑O�ʂɂ����Ă���
			SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
			SetWindowPos(hwnd, m_pApp->GetFullscreen() || m_pApp->GetAlwaysOnTop() ? HWND_TOPMOST : HWND_TOP,
			             0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
		} else {
			// TVTest�E�B���h�E�̔w�ʂɂ����Ă���
			SetWindowPos(hwnd, m_pApp->GetAppWindow(), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
		}
		return TRUE;
	case WM_POST_COMMENT:
		{
			TCHAR comm[POST_COMMENT_MAX + 1];
			if (GetDlgItemText(hwnd, IDC_CB_POST, comm, _countof(comm)) && comm[0] == TEXT('@')) {
				// ���[�J���R�}���h�Ƃ��ď���
				ProcessLocalPost(&comm[1]);
				return TRUE;
			}
			GetPostComboBoxText(comm, _countof(comm));
			if (GetTickCount() - lastPostTick_ < POST_COMMENT_INTERVAL) {
				OutputMessageLog(TEXT("Error:���e�Ԋu���Z�����܂��B"));
			} else if (lstrlen(comm) >= POST_COMMENT_MAX) {
				OutputMessageLog(TEXT("Error:���e�R�����g���������܂��B"));
			} else if (comm[0] && !lstrcmp(comm, lastPostComm_)) {
				OutputMessageLog(TEXT("Error:���e�R�����g���O��Ɠ����ł��B"));
			} else if (comm[0]) {
				static const std::regex reThread("^<thread[^>]*? thread=\"(\\d+)\"");
				std::cmatch mThread;
				if (!std::regex_search(commentServerResponse_, mThread, reThread) || !getflvUserID_[0]) {
					OutputMessageLog(TEXT("Error:�R�����g�T�[�o�ɐڑ����Ă��Ȃ������O�C�����Ă��܂���B"));
				} else {
					// �|�X�g�L�[�擾�J�n
					char szGet[_countof(cookie_) + 256];
					wsprintfA(szGet, "GET /api/v2/getpostkey?thread=%.15s&block_no=%d HTTP/1.1\r\n", mThread[1].str().c_str(), (lastChatNo_ + 1) / 100);
					AppendHttpHeader(szGet, "Host: ", JK_HOST_NAME, "\r\n");
					AppendHttpHeader(szGet, "Cookie: ", cookie_, "\r\n");
					AppendHttpHeader(szGet, "Connection: ", "close", "\r\n\r\n");
					if (postSocket_.Send(hwnd, WMS_POST, JK_HOST_NAME, 80, szGet)) {
						postBuf_.clear();
					}
				}
			}
		}
		return TRUE;
	case WM_SIZE:
		{
			RECT rcParent, rc;
			GetClientRect(hwnd, &rcParent);
			int padding = 4;
			HWND hItem = GetDlgItem(hwnd, IDC_CB_POST);
			GetWindowRect(hItem, &rc);
			MapWindowPoints(NULL, hwnd, reinterpret_cast<LPPOINT>(&rc), 2);
			if (!cookie_[0]) {
				// �N�b�L�[���ݒ肳��Ă��Ȃ���ΊԈႢ�Ȃ����e�s�\�Ȃ̂œ��̓{�b�N�X��\�����Ȃ�
				SetWindowPos(hItem, NULL, rc.left, rcParent.bottom, rcParent.right-rc.left*2, rc.bottom-rc.top, SWP_NOZORDER);
			} else {
				padding += 6 + static_cast<int>(SendMessage(hItem, CB_GETITEMHEIGHT, static_cast<WPARAM>(-1), 0));
				SetWindowPos(hItem, NULL, rc.left, rcParent.bottom-padding, rcParent.right-rc.left*2, rc.bottom-rc.top, SWP_NOZORDER);
				padding += 4;
			}
			hItem = GetDlgItem(hwnd, IDC_FORCELIST);
			GetWindowRect(hItem, &rc);
			MapWindowPoints(NULL, hwnd, reinterpret_cast<LPPOINT>(&rc), 2);
			SetWindowPos(hItem, NULL, 0, 0, rcParent.right-rc.left*2, rcParent.bottom-rc.top-padding, SWP_NOMOVE | SWP_NOZORDER);

			//---NicoJKKakoLog---
			GetWindowRect(GetDlgItem(hwnd,IDC_LISTVIEW), &rc);
			MapWindowPoints(NULL, hwnd, reinterpret_cast<LPPOINT>(&rc), 2);
			SetWindowPos(GetDlgItem(hwnd, IDC_LISTVIEW), NULL, 0, 0, rcParent.right - rc.left * 2, rcParent.bottom - rc.top - padding, SWP_NOMOVE | SWP_NOZORDER);
			//---NicoJKKakoLog---
		}
		break;
	}
	return FALSE;
}

// �X�g���[���R�[���o�b�N(�ʃX���b�h)
BOOL CALLBACK CNicoJK::StreamCallback(BYTE *pData, void *pClientData)
{
	CNicoJK *pThis = static_cast<CNicoJK*>(pClientData);
	int pid = ((pData[1]&0x1F)<<8) | pData[2];
	BYTE bTransportError = pData[1]&0x80;
	BYTE bPayloadUnitStart = pData[1]&0x40;
	BYTE bHasAdaptation = pData[3]&0x20;
	BYTE bHasPayload = pData[3]&0x10;
	BYTE bAdaptationLength = pData[4];
	BYTE bPcrFlag = pData[5]&0x10;

	// �V�[�N��|�[�Y�����o���邽�߂�PCR�𒲂ׂ�
	if (bHasAdaptation && bAdaptationLength >= 5 && bPcrFlag && !bTransportError) {
		DWORD pcr = (static_cast<DWORD>(pData[5+1])<<24) | (pData[5+2]<<16) | (pData[5+3]<<8) | pData[5+4];
		// �Q��PID��PCR������邱�ƂȂ�5��ʂ�PCR���o������΁A�Q��PID��ύX����
		if (pid != pThis->pcrPid_) {
			int i = 0;
			for (; pThis->pcrPids_[i] >= 0; ++i) {
				if (pThis->pcrPids_[i] == pid) {
					if (++pThis->pcrPidCounts_[i] >= 5) {
						pThis->pcrPid_ = pid;
					}
					break;
				}
			}
			if (pThis->pcrPids_[i] < 0 && i + 1 < _countof(pThis->pcrPids_)) {
				pThis->pcrPids_[i] = pid;
				pThis->pcrPidCounts_[i] = 1;
				pThis->pcrPids_[++i] = -1;
			}
		}
		if (pid == pThis->pcrPid_) {
			pThis->pcrPids_[0] = -1;
		}
		//dprintf(TEXT("CNicoJK::StreamCallback() PCR\n")); // DEBUG
		CBlockLock lock(&pThis->streamLock_);
		DWORD tick = GetTickCount();
		// 2�b�ȏ�PCR���擾�ł��Ă��Ȃ����|�[�Y�����?
		bool bReset = tick - pThis->pcrTick_ >= 2000;
		pThis->pcrTick_ = tick;
		if (pid == pThis->pcrPid_) {
			// 1�b�ȏ�PCR�����ł��遨�V�[�N?
			bReset = bReset || pcr - pThis->pcr_ >= 45000;
			pThis->pcr_ = pcr;
		}
		if (bReset) {
			pThis->ftTot_[0].dwHighDateTime = 0xFFFFFFFF;
			PostMessage(pThis->hForce_, WM_RESET_STREAM, 0, 0);
		}
	}

	// TOT�p�P�b�g�͒n��g�̎�����6�b��1���x
	// ARIB�K�i�ł͍Œ�30�b��1��
	if (pid == 0x14 && bPayloadUnitStart && bHasPayload && !bTransportError) {
		BYTE *pPayload = pData + 4;
		if (bHasAdaptation) {
			// �A�_�v�e�[�V�����t�B�[���h���X�L�b�v����
			if (bAdaptationLength > 182) {
				pPayload = NULL;
			} else {
				pPayload += 1 + bAdaptationLength;
			}
		}
		if (pPayload) {
			BYTE *pTable = pPayload + 1 + pPayload[0];
			// TOT or TDT (ARIB STD-B10)
			if (pTable + 7 < pData + 188 && (pTable[0] == 0x73 || pTable[0] == 0x70)) {
				// TOT������Tick�J�E���g���L�^����
				SYSTEMTIME st;
				FILETIME ft;
				if (AribToSystemTime(&pTable[3], &st) && SystemTimeToFileTime(&st, &ft)) {
					// UTC�ɕϊ�
					ft += -32400000LL * FILETIME_MILLISECOND;
					dprintf(TEXT("CNicoJK::StreamCallback() TOT\n")); // DEBUG
					CBlockLock lock(&pThis->streamLock_);
					pThis->ftTot_[1] = pThis->ftTot_[0];
					pThis->ftTot_[0] = ft;
					pThis->totTick_[1] = pThis->totTick_[0];
					pThis->totTick_[0] = GetTickCount();
				}
			}
		}
	}
	return TRUE;
}

TVTest::CTVTestPlugin *CreatePluginClass()
{
	return new CNicoJK();
}
