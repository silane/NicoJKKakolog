#pragma once

#include "NicoJKKakolog/NicoJKKakolog.h"

// �v���O�C���N���X
class CNicoJK : public TVTest::CTVTestPlugin
{
public:
	// ���X�g�{�b�N�X�̃��O�\���̍ő吔
	static const int COMMENT_TRIMEND = 1000;
	// �����ł���chat�^�O�̍ő啶����
	static const int CHAT_TAG_MAX = 2048;
	// �������X�g���X�V����Ԋu(����܂�Z��������_��!)
	static const int UPDATE_FORCE_INTERVAL = 20000;
	// �R�����g�T�[�o�ؒf���`�F�b�N���čĐڑ�����Ԋu(����܂�Z��������_��!)
	static const int JK_WATCHDOG_INTERVAL = 20000;
	// ���O�t�@�C���t�H���_�̍X�V���`�F�b�N����Ԋu
	static const int READ_LOG_FOLDER_INTERVAL = 3000;
	// �`�����l���ύX�Ȃǂ̌�ɓK���Ȏ���ID�̃`�F�b�N���s���܂ł̗P�\
	static const int SETUP_CURJK_DELAY = 3000;
	// ���e�ł���ő�R�����g������(���Ԃ���ՂɕύX���Ȃ��ق�������)
	static const int POST_COMMENT_MAX = 76;
	// �A������(�Z���ƋK�������Ƃ̃E���T)
	static const int POST_COMMENT_INTERVAL = 3000;
	// CTVTestPlugin
	CNicoJK();
	bool GetPluginInfo(TVTest::PluginInfo *pInfo);
	bool Initialize();
	bool Finalize();
private:
	struct SETTINGS {
		int hideForceWindow;
		int timerInterval;
		int halfSkipThreshold;
		int commentLineMargin;
		int commentFontOutline;
		int commentSize;
		int commentSizeMin;
		int commentSizeMax;
		TCHAR commentFontName[LF_FACESIZE];
		TCHAR commentFontNameMulti[LF_FACESIZE];
		bool bCommentFontBold;
		bool bCommentFontAntiAlias;
		int commentDuration;
		int logfileMode;
		TCHAR logfileDrivers[512];
		TCHAR nonTunerDrivers[512];
		TCHAR logfileFolder[MAX_PATH];
		TCHAR execGetCookie[1024];
		TCHAR mailDecorations[1024];
		bool bAnonymity;
		bool bUseOsdCompositor;
		bool bUseTexture;
		bool bUseDrawingThread;
		bool bShowRadio;
		bool bDoHalfClose;
		int maxAutoReplace;
		TCHAR abone[CCommentWindow::CHAT_TEXT_MAX];
		int dropLogfileMode;
		int forwardList[26];
		RECT rcForce;
		int forceOpacity;
		int commentOpacity;
		bool bSetRelative;
	};
	struct FORCE_ELEM {
		int jkID;
		int force;
		TCHAR name[64];
		FORCE_ELEM() {}
	};
	struct LOG_ELEM {
		SYSTEMTIME st;
		int no;
		TCHAR marker[28];
		TCHAR text[CCommentWindow::CHAT_TEXT_MAX];
		LOG_ELEM() {}
	};
	struct RPL_ELEM {
		int key;
		TCHAR section[32];
		TCHAR comment[32];
		TCHAR pattern[512];
		std::regex re;
		std::string fmt;
		RPL_ELEM() : key(0) {
			section[0] = comment[0] = pattern[0] = TEXT('\0');
		}
		bool IsEnabled() const { return IsCharUpper(pattern[0]) == FALSE; }
		void SetEnabled(bool b) { pattern[0] = b ? (TCHAR)(CharLower((LPTSTR)pattern[0])) : (TCHAR)(CharUpper((LPTSTR)pattern[0])); }
		bool AssignFromPattern();
		struct COMPARE {
			bool operator()(const RPL_ELEM &l, const RPL_ELEM &r) { return l.key < r.key; }
		};
	};
	bool TogglePlugin(bool bEnabled);
	static LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam);
	void ToggleStreamCallback(bool bSet);
	static unsigned int __stdcall SyncThread(void *pParam);
	void LoadFromIni();
	void SaveToIni();
	void LoadRplListFromIni(LPCTSTR section, std::vector<RPL_ELEM> *pRplList);
	void SaveRplListToIni(LPCTSTR section, const std::vector<RPL_ELEM> &rplList, bool bClearSection = true);
	HWND GetFullscreenWindow();
	HWND FindVideoContainer();
	DWORD GetCurrentNetworkServiceID();
	bool GetCurrentTot(FILETIME *pft);
	bool IsMatchDriverName(LPCTSTR drivers);
	void WriteToLogfile(int jkID, const char *text = NULL);
	bool ReadFromLogfile(int jkID, char *text = NULL, int len = 0, unsigned int tmToRead = 0);
	static LRESULT CALLBACK EventCallback(UINT Event, LPARAM lParam1, LPARAM lParam2, void *pClientData);
	static BOOL CALLBACK WindowMsgCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *pResult, void *pUserData);
	bool ProcessChatTag(const char *tag, bool bShow = true, int showDelay = 0);
	void OutputMessageLog(LPCTSTR text);
	void GetPostComboBoxText(LPTSTR comm, int commSize, LPTSTR mail = NULL, int mailSize = 0);
	void ProcessLocalPost(LPCTSTR comm);
	static INT_PTR CALLBACK ForceDialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	INT_PTR ForceDialogProcMain(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK StreamCallback(BYTE *pData, void *pClientData);

	// �ݒ�t�@�C���̖��O(Shlwapi�g���̂�MAX_PATH���傫�����Ă��Ӗ����Ȃ�)
	TCHAR szIniFileName_[MAX_PATH];
	SETTINGS s_;
	std::vector<NETWORK_SERVICE_ID_ELEM> ntsIDList_;
	std::vector<RPL_ELEM> rplList_;
	char cookie_[2048];
	bool bDragAcceptFiles_;

	// ������
	HWND hForce_;
	HHOOK hKeyboardHook_;
	bool bDisplayLogList_;
	std::vector<FORCE_ELEM> forceList_;
	std::list<LOG_ELEM> logList_;
	size_t logListDisplayedSize_;

	// �R�����g�`��E�B���h�E
	CCommentWindow commentWindow_;
	DWORD forwardTick_;
	HANDLE hSyncThread_;
	bool bQuitSyncThread_;
	bool bPendingTimerForward_;
	bool bHalfSkip_;
	bool bFlipFlop_;
	int forwardOffset_;
	int forwardOffsetDelta_;

	// �ʐM�p
	CAsyncSocket channelSocket_;
	CAsyncSocket jkSocket_;
	CAsyncSocket postSocket_;
	std::vector<char> channelBuf_;
	std::vector<char> jkBuf_;
	std::vector<char> postBuf_;
	int currentJKToGet_;
	int currentJK_;
	char jkLeaveThreadID_[16];
	int jkLeaveThreadCheck_;
	bool bConnectedToCommentServer_;
	char commentServerResponse_[CHAT_TAG_MAX];
	DWORD commentServerResponseTick_;
	char getflvUserID_[16];
	bool bGetflvIsPremium_;
	int lastChatNo_;
	DWORD lastPostTick_;
	TCHAR lastPostComm_[POST_COMMENT_MAX];

	// �ߋ����O�֌W
	bool bRecording_;
	bool bUsingLogfileDriver_;
	bool bSetStreamCallback_;
	int currentLogfileJK_;
	HANDLE hLogfile_;
	HANDLE hLogfileLock_;
	int currentReadLogfileJK_;
	CTextFileReader readLogfile_;
	char readLogText_[CHAT_TAG_MAX];
	unsigned int tmReadLogText_;
	DWORD readLogfileTick_;
	FILETIME ftTot_[2];
	DWORD totTick_[2];
	DWORD pcr_;
	DWORD pcrTick_;
	int pcrPid_;
	int pcrPids_[8];
	int pcrPidCounts_[8];
	CCriticalLock streamLock_;

	// �w��t�@�C���Đ�
	bool bSpecFile_;
	TCHAR tmpSpecFileName_[MAX_PATH];
	TCHAR dropFileName_[MAX_PATH];
	int dropFileTimeout_;



	//---NicoJKKakolog---
	NicoJKKakolog::NicoJKKakolog nicoJKKakolog;
};
