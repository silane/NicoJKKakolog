#pragma once

#include "OsdCompositor.h"

#pragma warning(disable : 4995)

class CCommentWindow
{
public:
	static const int DEFAULT_LINE_COUNT = 14;
	static const int DISPLAY_DURATION = 4000;
	static const int CHAT_TEXT_MAX = 256;
	// �\�����ׂ��R�����g���Ȃ��Ƃ��E�B���h�E���\���ɂ���܂ł̗P�\[�b]
	// (���C���[�h�E�B���h�E�͔킹�Ă��邾���ł�����Ȃ�̃R�X�g�������邽��)
	static const int AUTOHIDE_DELAY = 10;
	static const int FONT_SMALL_RATIO = 75;
	// �e�N�X�`���p�r�b�g�}�b�v�̕��̍ŏ��l(��������Ǐ��Q�Ɛ��̖ʂŏ\�������Ȓl)
	static const int TEXTURE_BITMAP_WIDTH_MIN = 640;
	enum CHAT_POSITION {
		CHAT_POS_DEFAULT,
		CHAT_POS_SHITA,
		CHAT_POS_UE,
	};
	enum CHAT_SIZE {
		CHAT_SIZE_DEFAULT,
		CHAT_SIZE_SMALL,
	};
	enum CHAT_ALIGN {
		CHAT_ALIGN_CENTER,
		CHAT_ALIGN_LEFT,
		CHAT_ALIGN_RIGHT,
	};
	bool Initialize(HINSTANCE hinst, bool *pbEnableOsdCompositor);
	void Finalize();
	CCommentWindow();
	~CCommentWindow();
	bool Create(HWND hwndParent);
	void Destroy();
	void SetStyle(LPCTSTR fontName, LPCTSTR fontNameMulti, bool bBold, bool bAntiAlias,
	              int fontOutline, bool bUseOsdCompositor, bool bUseTexture, bool bUseDrawingThread);
	void SetCommentSize(int size, int sizeMin, int sizeMax, int lineMargin);
	int GetDisplayDuration() const { return displayDuration_; }
	void SetDisplayDuration(int duration);
	BYTE GetOpacity() const { return opacity_; }
	void SetOpacity(BYTE opacity);
	void SetDebugFlags(int debugFlags);
	void OnParentMove();
	void OnParentSize();
	void AddChat(LPCTSTR text, COLORREF color, CHAT_POSITION position, CHAT_SIZE size = CHAT_SIZE_DEFAULT,
	             CHAT_ALIGN align = CHAT_ALIGN_CENTER, bool bInsertLast = false, BYTE backOpacity = 0, int delay = 0);
	void ScatterLatestChats(int duration);
	void ClearChat();
	void Forward(int duration);
	void Update();
	bool IsCreated() const { return hwnd_ != NULL; }
private:
	struct CHAT {
		DWORD pts;
		DWORD count;
		int line;
		Gdiplus::ARGB color;
		CHAT_POSITION position;
		bool bSmall;
		BYTE alignFactor;
		bool bInsertLast;
		bool bMultiLine;
		bool bDrew;
		int currentDrawWidth;
		int currentDrawHeight;
		TCHAR text[CHAT_TEXT_MAX];
		CHAT() {}
	};
	struct TEXTURE {
		bool bUsed;
		bool bSmall;
		RECT rc;
		Gdiplus::ARGB color;
		TCHAR text[CHAT_TEXT_MAX];
		TEXTURE() {}
		bool IsMatch(const CHAT &c) const { return c.color == color && c.bSmall == bSmall && !lstrcmp(c.text, text); }
	};
	bool AllocateWorkBitmap(int width, int height, bool *pbRealloc);
	void UpdateLayeredWindow();
	static unsigned int __stdcall DrawingThread(void *pParam);
	bool WaitForIdleDrawingThread();
	void UpdateChat();
	BOOL UpdateLayeredWindow(HWND hWnd, HDC hdcDst, POINT *pptDst, SIZE *psize, HDC hdcSrc, POINT *pptSrc,
	                         COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags, RECT *prcDirty);
	static BOOL CALLBACK UpdateCallback(void *pBits, const RECT *pSurfaceRect, int pitch, void *pClientData);
	void DrawChat(Gdiplus::Graphics &g, int width, int height, RECT *prcUnused, RECT *prcUnusedWoShita, bool *pbHasFirstDrawShita);
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HINSTANCE hinst_;
	ULONG_PTR gdiplusToken_;
	HMODULE hUser32_;
	BOOL (WINAPI *pfnUpdateLayeredWindowIndirect_)(HWND hWnd, const UPDATELAYEREDWINDOWINFO *pULWInfo);
	bool bWindows8_;
	bool bSse2Available_;
	HWND hwnd_;
	HWND hwndParent_;
	HBITMAP hbmWork_;
	void *pBits_;
	HDC hdcWork_;
	HANDLE hDrawingThread_;
	HANDLE hDrawingEvent_;
	HANDLE hDrawingIdleEvent_;
	volatile bool bQuitDrawingThread_;
	int commentSizeMin_;
	int commentSizeMax_;
	int lineCount_;
	double fontScale_;
	double fontSmallScale_;
	TCHAR fontName_[LF_FACESIZE];
	TCHAR fontNameMulti_[LF_FACESIZE];
	int fontStyle_;
	bool bAntiAlias_;
	BYTE opacity_;
	int fontOutline_;
	int displayDuration_;
	DWORD rts_;
	DWORD chatCount_;
	int currentWindowWidth_;
	std::list<CHAT> chatList_;
	std::list<CHAT> chatPoolList_;
	// chatList_(���X�g�\���̂�),chatPoolList_��ی�
	CCriticalLock chatLock_;
	int autoHideCount_;
	COsdCompositor osdCompositor_;
	bool bUseOsd_;
	bool bShowOsd_;
	bool bUseTexture_;
	bool bUseDrawingThread_;
	Gdiplus::Bitmap *pTextureBitmap_;
	Gdiplus::Graphics *pgTexture_;
	int currentTextureHeight_;
	std::list<TEXTURE> textureList_;
	RECT rcUnused_;
	RECT rcUnusedWoShita_;
	RECT rcUnusedIntersect_;
	RECT rcDirty_;
	bool bForceRefreshDirty_;
	int debugFlags_;
};
