#include "stdafx.h"
#include "Util.h"
#include "CommentWindow.h"
#include <emmintrin.h>

#ifndef ASSERT
#include <cassert>
#define ASSERT assert
#endif

#if 0 // �A�Z���u�������p
#define MAGIC_NUMBER(x) { g_dwMagic=(x); }
static DWORD g_dwMagic;
#else
#define MAGIC_NUMBER
#endif

static void ApplyOpacityPlain(DWORD *pBits, int range, BYTE opacityA, BYTE opacityRGB)
{
	MAGIC_NUMBER(0x12452356);
	for (int i = 0; i < range; ++i) {
		DWORD x = pBits[i];
		pBits[i] = ((x>>24)*opacityA&0xFF00)<<16 | ((x>>16&0xFF)*opacityRGB&0xFF00)<<8 | ((x>>8&0xFF)*opacityRGB&0xFF00) | ((x&0xFF)*opacityRGB)>>8;
	}
}

static void ApplyOpacitySse2(DWORD *pBits, int range, BYTE opacityA, BYTE opacityRGB)
{
	int top = 3 - reinterpret_cast<ULONG_PTR>(pBits + 3) / sizeof(DWORD) % 4;
	ApplyOpacityPlain(pBits, min(top, range), opacityA, opacityRGB);

	// ApplyOpacityPlain()�ɑ΂��ĒP�̂Ŏ���6.8�{����
	MAGIC_NUMBER(0x13243546);
	__m128i *pAligned = reinterpret_cast<__m128i*>(pBits + top);
	int bottom = (range - top) / 4;
	__m128i zero = _mm_setzero_si128();
	__m128i mult = _mm_set_epi16(opacityA, opacityRGB, opacityRGB, opacityRGB, opacityA, opacityRGB, opacityRGB, opacityRGB);
	for (int i = 0; i < bottom; ++i) {
		__m128i x = _mm_load_si128(pAligned + i);
		__m128i ly = _mm_mulhi_epu16(_mm_unpacklo_epi8(zero, x), mult);
		__m128i hy = _mm_mulhi_epu16(_mm_unpackhi_epi8(zero, x), mult);
		_mm_store_si128(pAligned + i, _mm_packus_epi16(ly, hy));
	}
	if (range > top) {
		ApplyOpacityPlain(pBits + top + bottom * 4, range - (top + bottom * 4), opacityA, opacityRGB);
	}
}

static void ApplyOpacity(DWORD *pBits, int range, BYTE opacityA, BYTE opacityRGB, bool bUseSse2)
{
	if (bUseSse2) {
		ApplyOpacitySse2(pBits, range, opacityA, opacityRGB);
	} else {
		ApplyOpacityPlain(pBits, range, opacityA, opacityRGB);
	}
}

bool CCommentWindow::Initialize(HINSTANCE hinst, bool *pbEnableOsdCompositor)
{
	if (!hinst_) {
		WNDCLASSEX wc = {0};
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.lpfnWndProc = WndProc;
		wc.hInstance = hinst;
		wc.lpszClassName = TEXT("ru.jk.comment");
		if (RegisterClassEx(&wc) == 0) {
			return false;
		}
		// TVTest�{�̂�Gdiplus���g���Ă���̂Œx�����[�h�͂��Ȃ�
		Gdiplus::GdiplusStartupInput si;
		if (Gdiplus::GdiplusStartup(&gdiplusToken_, &si, NULL) != Gdiplus::Ok) {
			return false;
		}
		hinst_ = hinst;

		if (pbEnableOsdCompositor && *pbEnableOsdCompositor) {
			// �[���łȂ�OSD��L���ɂ���
			// ���ۂɎg�����ǂ�����SetStyle()�Ō��߂�
			*pbEnableOsdCompositor = osdCompositor_.Initialize();
			osdCompositor_.SetUpdateCallback(UpdateCallback, this);
		}

		OSVERSIONINFO vi;
		vi.dwOSVersionInfoSize = sizeof(vi);
		bWindows8_ = false;
		if (GetVersionEx(&vi)) {
			bWindows8_ = vi.dwMajorVersion==6 && vi.dwMinorVersion==2;
			if (vi.dwMajorVersion >= 6 && (hUser32_ = LoadLibrary(TEXT("user32.dll"))) != NULL) {
				// ����API��Vista�ȍ~�ɑ��݂��邪Vista�̎����̓o�O���܂ނ炵��(KB955688)�̂Œ���
				(void*&)pfnUpdateLayeredWindowIndirect_ = GetProcAddress(hUser32_, "UpdateLayeredWindowIndirect");
			}
		}
		bSse2Available_ = IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE) != FALSE;
	}
	return true;
}

void CCommentWindow::Finalize()
{
	pfnUpdateLayeredWindowIndirect_ = NULL;
	if (hUser32_) {
		FreeLibrary(hUser32_);
		hUser32_ = NULL;
	}
	if (hinst_) {
        osdCompositor_.Uninitialize();
		Gdiplus::GdiplusShutdown(gdiplusToken_);
		UnregisterClass(TEXT("ru.jk.comment"), hinst_);
		hinst_ = NULL;
	}
}

CCommentWindow::CCommentWindow()
	: hinst_(NULL)
	, gdiplusToken_(0)
	, hUser32_(NULL)
	, pfnUpdateLayeredWindowIndirect_(NULL)
	, bWindows8_(false)
	, bSse2Available_(false)
	, hwnd_(NULL)
	, hwndParent_(NULL)
	, hbmWork_(NULL)
	, pBits_(NULL)
	, hdcWork_(NULL)
	, hDrawingThread_(NULL)
	, hDrawingEvent_(NULL)
	, hDrawingIdleEvent_(NULL)
	, bQuitDrawingThread_(false)
	, commentSizeMin_(1)
	, commentSizeMax_(INT_MAX)
	, lineCount_(DEFAULT_LINE_COUNT)
	, fontScale_(1.0)
	, fontSmallScale_(1.0)
	, fontStyle_(Gdiplus::FontStyleRegular)
	, bAntiAlias_(false)
	, opacity_(255)
	, fontOutline_(0)
	, displayDuration_(DISPLAY_DURATION)
	, rts_(0)
	, chatCount_(0)
	, currentWindowWidth_(-1)
	, autoHideCount_(0)
	, bUseOsd_(false)
	, bShowOsd_(false)
	, bUseTexture_(false)
	, bUseDrawingThread_(false)
	, pTextureBitmap_(NULL)
	, pgTexture_(NULL)
	, currentTextureHeight_(0)
	, bForceRefreshDirty_(false)
	, debugFlags_(0)
{
	fontName_[0] = TEXT('\0');
	fontNameMulti_[0] = TEXT('\0');
}

CCommentWindow::~CCommentWindow()
{
	Destroy();
	Finalize();
}

bool CCommentWindow::Create(HWND hwndParent)
{
	if (hwnd_) {
		return true;
	}
	if (hinst_ && hwndParent) {
		// ���ۂɂ͐e�E�B���h�E�ł͂Ȃ�
		hwndParent_ = hwndParent;
		CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
		               TEXT("ru.jk.comment"), NULL, WS_POPUP, 0, 0, 0, 0, hwndParent_, NULL, hinst_, this);
		if (hwnd_) {
			if (!bUseOsd_ && bUseDrawingThread_) {
				hDrawingEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
				hDrawingIdleEvent_ = CreateEvent(NULL, TRUE, TRUE, NULL);
				if (hDrawingEvent_ && hDrawingIdleEvent_) {
					bQuitDrawingThread_ = false;
					hDrawingThread_ = reinterpret_cast<HANDLE>(::_beginthreadex(NULL, 0, DrawingThread, this, 0, NULL));
					if (hDrawingThread_) {
						SetThreadPriority(hDrawingThread_, THREAD_PRIORITY_ABOVE_NORMAL);
					}
				}
			}
			osdCompositor_.SetContainerWindow(hwndParent_);
			OnParentSize();
			if (!bUseOsd_) {
				ShowWindow(hwnd_, SW_SHOWNOACTIVATE);
			}
			return true;
		}
	}
	return false;
}

void CCommentWindow::Destroy()
{
	if (hDrawingThread_) {
		bQuitDrawingThread_ = true;
		SetEvent(hDrawingEvent_);
		if (WaitForSingleObject(hDrawingThread_, 30000) == WAIT_TIMEOUT) {
			TerminateThread(hDrawingThread_, 1);
		}
		CloseHandle(hDrawingThread_);
		hDrawingThread_ = NULL;
	}
	if (hDrawingIdleEvent_) {
		CloseHandle(hDrawingIdleEvent_);
		hDrawingIdleEvent_ = NULL;
	}
	if (hDrawingEvent_) {
		CloseHandle(hDrawingEvent_);
		hDrawingEvent_ = NULL;
	}
	if (hwnd_) {
		DestroyWindow(hwnd_);
	}
	if (hbmWork_) {
		DeleteObject(hbmWork_);
		hbmWork_ = NULL;
	}
	if (hdcWork_) {
		DeleteDC(hdcWork_);
		hdcWork_ = NULL;
	}
	if (bShowOsd_) {
		osdCompositor_.DeleteTexture(0, 0);
		bShowOsd_ = false;
		osdCompositor_.UpdateSurface();
	}
	autoHideCount_ = 0;

	delete pgTexture_;
	delete pTextureBitmap_;
	pgTexture_ = NULL;
	pTextureBitmap_ = NULL;
}

void CCommentWindow::SetStyle(LPCTSTR fontName, LPCTSTR fontNameMulti, bool bBold, bool bAntiAlias,
                              int fontOutline, bool bUseOsdCompositor, bool bUseTexture, bool bUseDrawingThread)
{
	WaitForIdleDrawingThread();
	ClearChat();
	lstrcpyn(fontName_, fontName, _countof(fontName_));
	lstrcpyn(fontNameMulti_, fontNameMulti, _countof(fontNameMulti_));
	fontStyle_ = bBold ? Gdiplus::FontStyleBold : Gdiplus::FontStyleRegular;
	bAntiAlias_ = bAntiAlias;
	fontOutline_ = max(fontOutline, 0);
	bUseOsd_ = bUseOsdCompositor;
	bUseTexture_ = bUseTexture;
	bUseDrawingThread_ = bUseDrawingThread;
}

void CCommentWindow::SetCommentSize(int size, int sizeMin, int sizeMax, int lineMargin)
{
	WaitForIdleDrawingThread();
	ClearChat();
	commentSizeMin_ = max(sizeMin, 1);
	commentSizeMax_ = max(sizeMax, 1);
	lineCount_ = max(DEFAULT_LINE_COUNT * 10000 / min(max(size,10),1000) / min(max(lineMargin,10),1000), 1);
	fontScale_ = 100.0 / min(max(lineMargin,10),1000);
	fontSmallScale_ = fontScale_ * FONT_SMALL_RATIO / 100;
}

void CCommentWindow::SetDisplayDuration(int duration)
{
	WaitForIdleDrawingThread();
	displayDuration_ = min(max(duration, 100), 100000);
}

void CCommentWindow::SetOpacity(BYTE opacity)
{
	WaitForIdleDrawingThread();
	opacity_ = opacity;
}

void CCommentWindow::SetDebugFlags(int debugFlags)
{
	WaitForIdleDrawingThread();
	debugFlags_ = debugFlags;
}

// ��Ɨp�r�b�g�}�b�v���m��
bool CCommentWindow::AllocateWorkBitmap(int width, int height, bool *pbRealloc)
{
	if (hbmWork_) {
		BITMAP bm;
		if (GetObject(hbmWork_,sizeof(BITMAP),&bm) && bm.bmWidth==width && bm.bmHeight==height) {
			*pbRealloc = false;
			return true;
		}
		DeleteObject(hbmWork_);
		hbmWork_ = NULL;
	}
	if (hdcWork_) {
		DeleteDC(hdcWork_);
		hdcWork_ = NULL;
	}
	//�f�o�C�X�R���e�L�X�g�������Ɋm��
	if (!hwnd_) {
		return false;
	}
	HDC hdc = GetDC(hwnd_);
	hdcWork_ = CreateCompatibleDC(hdc);
	ReleaseDC(hwnd_, hdc);
	if (!hdcWork_) {
		return false;
	}
	BITMAPINFO bmi = {0};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	hbmWork_ = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &pBits_, NULL, 0);
	*pbRealloc = true;
	return hbmWork_ != NULL;
}

void CCommentWindow::OnParentMove()
{
	if (hwnd_) {
		RECT rc;
		GetWindowRect(hwndParent_, &rc);
		SetWindowPos(hwnd_, NULL, rc.left, rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

void CCommentWindow::OnParentSize()
{
	if (hwnd_ && bUseOsd_) {
		if (bShowOsd_) {
			osdCompositor_.DeleteTexture(0, 0);
			bShowOsd_ = false;
			RECT rc;
			if (osdCompositor_.GetSurfaceRect(&rc) && rc.right-rc.left > 0 && rc.bottom-rc.top > 0) {
				// ����ƉE���Ƀe�N�X�`���o�^���邱�Ƃ�OSD�̕`��̈�𓮉�S�̂Ɋg����
				BITMAPINFO bmi = {0};
				bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bmi.bmiHeader.biWidth = 1;
				bmi.bmiHeader.biHeight = 1;
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 32;
				bmi.bmiHeader.biCompression = BI_RGB;
				void *pBits;
				HBITMAP hbm = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
				if (hbm) {
					*static_cast<DWORD*>(pBits) = 0/*0xFF00FF00*/;
					osdCompositor_.AddTexture(hbm, 0, 0, true, 0);
					osdCompositor_.AddTexture(hbm, rc.right-rc.left-1, rc.bottom-rc.top-1, true, 0);
					DeleteObject(hbm);
					bShowOsd_ = true;
				}
			}
		}
	} else if (hwnd_ && !bUseOsd_) {
		RECT rc;
		GetWindowRect(hwndParent_, &rc);
		SetWindowPos(hwnd_, NULL, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

void CCommentWindow::AddChat(LPCTSTR text, COLORREF color, CHAT_POSITION position,
                             CHAT_SIZE size, CHAT_ALIGN align, bool bInsertLast, BYTE backOpacity, int delay)
{
	if (hwnd_) {
		CHAT c;
		c.pts = rts_ + delay;
		c.count = ++chatCount_;
		c.line = INT_MAX;
		c.color = Gdiplus::Color::MakeARGB(bAntiAlias_ ? backOpacity : backOpacity ? 255 : 0, GetRValue(color), GetGValue(color), GetBValue(color));
		c.position = position;
		c.bSmall = size != CHAT_SIZE_DEFAULT;
		c.alignFactor = position==CHAT_POS_DEFAULT || align==CHAT_ALIGN_LEFT ? 0 : align==CHAT_ALIGN_RIGHT ? 2 : 1;
		c.bInsertLast = position!=CHAT_POS_DEFAULT && bInsertLast;
		lstrcpyn(c.text, text, _countof(c.text));
		c.bMultiLine = StrChr(c.text, TEXT('\n')) != NULL;
		c.bDrew = false;
		// �ꎞ���X�g�ɒǉ�(�`�掞��chatList_�Ƀ}�[�W)
		CBlockLock lock(&chatLock_);
		chatPoolList_.push_back(c);
	}
}

// �Ō�ɒǉ����ꂽ�������̃R�����g�̕\���^�C�~���O��duration�͈͓̔��œK���ɎU�炷
void CCommentWindow::ScatterLatestChats(int duration)
{
	if (hwnd_ && duration > 0) {
		CBlockLock lock(&chatLock_);
		std::list<CHAT>::iterator it = chatPoolList_.begin();
		DWORD latestPts = 0;
		int latestNum = 0;
		for (; it != chatPoolList_.end(); ++it, ++latestNum) {
			if (latestPts != it->pts) {
				latestPts = it->pts;
				latestNum = 0;
			}
		}
		if (latestNum > 0) {
			for (int i = latestNum - 1; i >= 0; --i) {
				(--it)->pts += duration * i / latestNum;
			}
		}
	}
}

void CCommentWindow::ClearChat()
{
	WaitForIdleDrawingThread();
	chatList_.clear();
	chatPoolList_.clear();
	textureList_.clear();
	bForceRefreshDirty_ = true;
}

#define DWORD_MSB(x) ((x) & 0x80000000)
#define DWORD_DIFF(a,b) (DWORD_MSB((a)-(b)) ? -(int)((b)-(a)) : (int)((a)-(b)))

void CCommentWindow::Forward(int duration)
{
	WaitForIdleDrawingThread();

	if (duration > 0) {
		rts_ += duration;
		std::list<CHAT>::const_iterator it = chatList_.begin();
		while (it != chatList_.end()) {
			// �\�������؂�̃R�����g��ǂ��o��
			if (DWORD_MSB(it->pts + displayDuration_ - rts_)) {
				if (it->position == CHAT_POS_SHITA && it->bDrew) {
					bForceRefreshDirty_ = true;
				}
				it = chatList_.erase(it);
			} else {
				++it;
			}
		}
		it = chatPoolList_.begin();
		while (it != chatPoolList_.end()) {
			if (DWORD_MSB(it->pts + displayDuration_ - rts_)) {
				it = chatPoolList_.erase(it);
			} else {
				++it;
			}
		}
	}
}

void CCommentWindow::Update()
{
	WaitForIdleDrawingThread();

	if (hwnd_ && bUseOsd_) {
		// OSD�ɕ`��
		if (bShowOsd_) {
			osdCompositor_.UpdateSurface();
		} else {
			if ((!chatList_.empty() || !chatPoolList_.empty()) && IsWindowVisible(hwndParent_)) {
				bShowOsd_ = true;
				OnParentSize();
				if (bShowOsd_) {
					osdCompositor_.UpdateSurface();
				}
			}
		}
	} else if (hwnd_ && !bUseOsd_) {
		// ���C���[�h�E�B���h�E�ɕ`��
		if (IsWindowVisible(hwnd_)) {
			UpdateLayeredWindow();
		} else {
			if ((!chatList_.empty() || !chatPoolList_.empty()) && IsWindowVisible(hwndParent_)) {
				ShowWindow(hwnd_, SW_SHOWNOACTIVATE);
				UpdateLayeredWindow();
			}
		}
	}
}

void CCommentWindow::UpdateLayeredWindow()
{
	ASSERT(!hDrawingThread_ || WaitForSingleObject(hDrawingIdleEvent_, 0) == WAIT_OBJECT_0);

	// GDI�I�u�W�F�N�g���m��
	RECT rc;
	GetClientRect(hwnd_, &rc);
	int width = rc.right;
	int height = rc.bottom;
	bool bRealloc;
	if (width <= 0 || height <= 0 || !AllocateWorkBitmap(width, height, &bRealloc)) {
		return;
	}
	HBITMAP hbmOld = static_cast<HBITMAP>(SelectObject(hdcWork_, hbmWork_));

	if (bRealloc) {
		Gdiplus::Graphics g(hdcWork_);
		g.Clear(bAntiAlias_ ? Gdiplus::Color::Transparent : Gdiplus::Color(12, 12, 12));
		SetRect(&rcUnused_, 0, 0, width, height);
		SetRect(&rcUnusedWoShita_, 0, 0, width, height);
		SetRect(&rcUnusedIntersect_, 0, 0, width, height);
		SetRect(&rcDirty_, 0, 0, width, height);
	}
	// 1�O�̕`�挋�ʂ𑦍��ɓK�p���邱�Ƃœ����������\����������
	SIZE sz = {width, height};
	POINT ptSrc = {0, 0};
	BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	// �_�[�e�B�̈���w������ƊՎU�Ƃ��������Ŏ���25%���y��
	HDC hdc = GetDC(hwnd_);
	UpdateLayeredWindow(hwnd_, hdc, NULL, &sz, hdcWork_, &ptSrc, RGB(12, 12, 12), &blend, bAntiAlias_ ? ULW_ALPHA : ULW_COLORKEY, &rcDirty_);
	ReleaseDC(hwnd_, hdc);
	SelectObject(hdcWork_, hbmOld);

	if (hDrawingThread_) {
		// ���Ƃ̓X���b�h�ɔC����
		ResetEvent(hDrawingIdleEvent_);
		SetEvent(hDrawingEvent_);
	} else {
		UpdateChat();
	}
}

// �`��X���b�h
unsigned int __stdcall CCommentWindow::DrawingThread(void *pParam)
{
	CCommentWindow *pThis = static_cast<CCommentWindow*>(pParam);
	while (WaitForSingleObject(pThis->hDrawingEvent_, INFINITE) == WAIT_OBJECT_0 && !pThis->bQuitDrawingThread_) {
		pThis->UpdateChat();
		SetEvent(pThis->hDrawingIdleEvent_);
	}
	pThis->bQuitDrawingThread_ = true;
	return 0;
}

// �`��X���b�h���A�C�h����ԂɂȂ�̂�҂�
bool CCommentWindow::WaitForIdleDrawingThread()
{
	return !hDrawingThread_ || bQuitDrawingThread_ || WaitForSingleObject(hDrawingIdleEvent_, INFINITE) == WAIT_OBJECT_0;
}

void CCommentWindow::UpdateChat()
{
	BITMAP bm;
	if (!GetObject(hbmWork_, sizeof(BITMAP), &bm) || bm.bmWidth <= 0 || bm.bmHeight <= 0) {
		return;
	}
	int width = bm.bmWidth;
	int height = bm.bmHeight;
	HBITMAP hbmOld = static_cast<HBITMAP>(SelectObject(hdcWork_, hbmWork_));

	// GDI+�`��X�R�[�v
	{
		Gdiplus::Graphics g(hdcWork_);
		// ��������(����or�J���[�L�[)�ɂ���Ĕw�i�F��ς���
		Gdiplus::SolidBrush br(bAntiAlias_ ? Gdiplus::Color::Transparent : Gdiplus::Color(12, 12, 12));
		if (debugFlags_ & 8) {
			// Debug:�������̈�ɐF������
			static DWORD s_count;
			br.SetColor(Gdiplus::Color(128, 0, ++s_count * 2 % 256, 0));
		}
		g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
		// 60fps������ɂȂ�Ə������R�X�g���n���ɂł��Ȃ��̂Ŏg�p�ςݕ��������N���A(�ՎU�Ƃ��������ł͎���15%�y��)
		g.FillRectangle(&br, 0, 0, width, rcUnused_.top);
		g.FillRectangle(&br, 0, rcUnused_.bottom, width, height - rcUnused_.bottom);
		RECT rcLast = rcUnusedWoShita_;
		bool bHasFirstDrawShita;
		DrawChat(g, width, height, &rcUnused_, &rcUnusedWoShita_, &bHasFirstDrawShita);
		// �N���A+�g�p�ς�==�_�[�e�B�̈�
		IntersectRect(&rcDirty_, &rcLast, &rcUnusedWoShita_);
		// �g�p�ςݗ̈��ώZ
		RECT rcTmp = rcUnusedIntersect_;
		IntersectRect(&rcUnusedIntersect_, &rcTmp, &rcUnused_);
		// ���͐Î~�R�������Ȃ��̂Ń_�[�e�B�̈�X�V���T�{��
		if (bHasFirstDrawShita || bForceRefreshDirty_) {
			rcDirty_ = rcUnusedIntersect_;
			rcUnusedIntersect_ = rcUnused_;
			bForceRefreshDirty_ = false;
		}
		SetRect(&rcDirty_, 0, rcDirty_.bottom<height && rcDirty_.top<=0 ? rcDirty_.bottom : 0,
		        width, rcDirty_.bottom>=height ? rcDirty_.top : height);
	}
	SelectObject(hdcWork_, hbmOld);

	if (bAntiAlias_ && opacity_ != 255) {
		// �s�����x��K�p(BLENDFUNCTION�ł��\�����ǂ₽��d��)
		ApplyOpacity(static_cast<DWORD*>(pBits_), width * (height - rcUnused_.bottom), opacity_, opacity_, bSse2Available_);
		ApplyOpacity(static_cast<DWORD*>(pBits_) + width * (height - rcUnused_.top), width * rcUnused_.top, opacity_, opacity_, bSse2Available_);
	}
}

BOOL CCommentWindow::UpdateLayeredWindow(HWND hWnd, HDC hdcDst, POINT *pptDst, SIZE *psize, HDC hdcSrc, POINT *pptSrc,
                                         COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags, RECT *prcDirty)
{
	if (pfnUpdateLayeredWindowIndirect_) {
		UPDATELAYEREDWINDOWINFO info;
		info.cbSize = sizeof(info);
		info.hdcDst = hdcDst;
		info.pptDst = pptDst;
		info.psize = psize;
		info.hdcSrc = hdcSrc;
		info.pptSrc = pptSrc;
		info.crKey = crKey;
		info.pblend = pblend;
		info.dwFlags = dwFlags;
		info.prcDirty = prcDirty;
		return pfnUpdateLayeredWindowIndirect_(hWnd, &info);
	} else {
		return ::UpdateLayeredWindow(hWnd, hdcDst, pptDst, psize, hdcSrc, pptSrc, crKey, pblend, dwFlags & ~ULW_EX_NORESIZE);
	}
}

BOOL CALLBACK CCommentWindow::UpdateCallback(void *pBits, const RECT *pSurfaceRect, int pitch, void *pClientData)
{
	CCommentWindow *pThis = static_cast<CCommentWindow*>(pClientData);
	int width = pSurfaceRect->right - pSurfaceRect->left;
	int height = pSurfaceRect->bottom - pSurfaceRect->top;

	// GDI+�`��X�R�[�v
	if (pThis->bShowOsd_ && width > 0 && height > 0) {
		RECT rcUnused, rcUnusedWoShita;
		bool bHasFirstDrawShita;
		{
			// Premult�ɂ���Ǝ�����10%���x�y������OsdCompositor�͔�Premult�Ȃ̂Ŕ������������Ȃ��Ƃ������g��
			Gdiplus::Bitmap bitmap(width, height, pitch, !pThis->bAntiAlias_ ? PixelFormat32bppPARGB : PixelFormat32bppARGB, static_cast<BYTE*>(pBits));
			Gdiplus::Graphics g(&bitmap);
			pThis->DrawChat(g, width, height, &rcUnused, &rcUnusedWoShita, &bHasFirstDrawShita);
		}
		if (pThis->opacity_ != 255 && pitch % 4 == 0) {
			// �s�����x��K�p
			ApplyOpacity(static_cast<DWORD*>(pBits), pitch / 4 * rcUnused.top, pThis->opacity_, 255, pThis->bSse2Available_);
			ApplyOpacity(static_cast<DWORD*>(pBits) + pitch / 4 * rcUnused.bottom, pitch / 4 * (height - rcUnused.bottom), pThis->opacity_, 255, pThis->bSse2Available_);
		}
	}
	return FALSE;
}

void CCommentWindow::DrawChat(Gdiplus::Graphics &g, int width, int height, RECT *prcUnused, RECT *prcUnusedWoShita, bool *pbHasFirstDrawShita)
{
	{
		// �ŏ��E�ő啶���T�C�Y�ɉ����ĕ\�����C�����𑝌�����
		int lineCountMax = static_cast<int>(fontScale_ * height / commentSizeMin_);
		int lineCountMin = static_cast<int>(fontScale_ * height / commentSizeMax_);
		int actLineCount = min(max(lineCount_, lineCountMin), max(lineCountMax, 1));

		int textureWidth = max(width, TEXTURE_BITMAP_WIDTH_MIN);
		if (bUseTexture_) {
			// �e�N�X�`���p�r�b�g�}�b�v���m��
			if (!pTextureBitmap_ || (int)pTextureBitmap_->GetWidth() != textureWidth || (int)pTextureBitmap_->GetHeight() != height) {
				delete pgTexture_;
				delete pTextureBitmap_;
				// Premult�̕������Ȃ�y���� (�Q�l: http://www.codeproject.com/Tips/66909/Rendering-fast-with-GDI-What-to-do-and-what-not-to )
				pTextureBitmap_ = new Gdiplus::Bitmap(textureWidth, height, PixelFormat32bppPARGB);
				pgTexture_ = new Gdiplus::Graphics(pTextureBitmap_);
				pgTexture_->SetTextRenderingHint(bAntiAlias_ ? Gdiplus::TextRenderingHintAntiAlias : Gdiplus::TextRenderingHintSingleBitPerPixel);
				textureList_.clear();
			}
		}
		g.SetTextRenderingHint(bAntiAlias_ ? Gdiplus::TextRenderingHintAntiAlias : Gdiplus::TextRenderingHintSingleBitPerPixel);
		Gdiplus::REAL fontEm = (Gdiplus::REAL)(fontScale_ * height / actLineCount * 0.75);
		Gdiplus::REAL fontEmSmall = (Gdiplus::REAL)(fontSmallScale_ * height / actLineCount * 0.75);
		Gdiplus::Font font(fontName_, fontEm, fontStyle_);
		Gdiplus::Font fontSmall(fontName_, fontEmSmall, fontStyle_);
		// �����s�p�t�H���g
		Gdiplus::Font tmpFontMulti(fontNameMulti_, fontEm, fontStyle_);
		Gdiplus::Font tmpFontMultiSmall(fontNameMulti_, fontEmSmall, fontStyle_);
		// �\�Ȃ瓯���I�u�W�F�N�g���Q�Ƃ��Ă���(�������Ƃ��邩������Ȃ��̂�)
		bool bSameMultiFont = !lstrcmp(fontName_, fontNameMulti_);
		const Gdiplus::Font &fontMulti = bSameMultiFont ? font : tmpFontMulti;
		const Gdiplus::Font &fontMultiSmall = bSameMultiFont ? fontSmall : tmpFontMultiSmall;

		{
		CBlockLock lock(&chatLock_);

		// �V�����R�����g������Ώd�Ȃ�Ȃ��悤�Ƀ��X�g���Ĕz�u����
		while (!chatPoolList_.empty()) {
			CHAT c = chatPoolList_.front();
			chatPoolList_.pop_front();
			// ���ۂ̕`��T�C�Y���v��
			const Gdiplus::Font *pFont = c.bMultiLine ? &(c.bSmall ? fontMultiSmall : fontMulti) : &(c.bSmall ? fontSmall : font);
			Gdiplus::RectF rcDraw;
			if (g.MeasureString(c.text, -1, pFont, Gdiplus::PointF(0, 0), &rcDraw) != Gdiplus::Ok) {
				continue;
			}
			c.currentDrawWidth = (int)rcDraw.Width;
			c.currentDrawHeight = (int)rcDraw.Height;
			// �R�����g�̕\�����C����ݒ肷��
			// ���R�����͕����s�Ή��̂��߂ɉ��[���\���ł��郉�C������ɔz�u
			c.line = c.position == CHAT_POS_SHITA && c.bMultiLine ? actLineCount - 1 - (int)((double)rcDraw.Height / height * actLineCount) :
			         c.position == CHAT_POS_SHITA ? actLineCount - 1 : 0;
			std::list<CHAT>::iterator it = chatList_.begin();
			std::list<CHAT>::const_iterator itLast = chatList_.end();
			for (; it != chatList_.end(); ++it) {
				if (it->line != INT_MAX && c.position == it->position && c.alignFactor == it->alignFactor) {
					if (c.bInsertLast) {
						// �Ō�ɒǉ����ꂽ�R�����g��T��
						if (itLast == chatList_.end() || DWORD_MSB(itLast->count - it->count)) {
							itLast = it;
						}
					} else if (c.position == CHAT_POS_SHITA) {
						// ���ɂ���R�����g�قǃ��X�g�O���ɑ}������Ă���
						if (c.line > it->line) {
							break;
						} else if (c.line == it->line) {
							// c���\���J�n���鎞�_��*it�������Ă���Ώd�Ȃ�Ȃ�
							if (DWORD_DIFF(c.pts, it->pts) > displayDuration_) {
								break;
							}
							c.line--;
						}
					} else if (c.position == CHAT_POS_UE) {
						// ��ɂ���R�����g�قǃ��X�g�O���ɑ}������Ă���
						if (c.line < it->line) {
							break;
						} else if (c.line == it->line) {
							if (DWORD_DIFF(c.pts, it->pts) > displayDuration_) {
								break;
							}
							c.line++;
						}
					} else {
						// ��ɂ���R�����g�قǃ��X�g�O���ɑ}������Ă���
						// �O�̂ɒǂ����Ȃ���΂���
						if (c.line < it->line) {
							break;
						} else if (c.line == it->line) {
							// c���\���J�n���鎞�_��*it�̉E�[���E�B���h�E���ɂ���Ώd�Ȃ�Ȃ�
							if (DWORD_DIFF(c.pts, it->pts) * (width + it->currentDrawWidth) / displayDuration_ - it->currentDrawWidth > 0) {
								// *it���\���������}���鎞�_��c�̍��[���E�B���h�E���ɂ���Βǂ����Ȃ�
								if (width - DWORD_DIFF(it->pts + displayDuration_, c.pts) * (width + c.currentDrawWidth) / displayDuration_ > 0) {
									break;
								}
							}
							c.line++;
						}
					}
				}
			}
			if (itLast != chatList_.end()) {
				if (c.position == CHAT_POS_SHITA) {
					if (itLast->line > 0) {
						// itLast�̒��O�ɑ}������
						c.line = itLast->line - 1;
					}
				} else {
					if (itLast->line < actLineCount - 1) {
						c.line = itLast->line + 1;
					}
				}
				// �}���ʒu���Č���
				for (it = chatList_.begin(); it != chatList_.end(); ++it) {
					if (it->line != INT_MAX && c.position == it->position && c.alignFactor == it->alignFactor &&
					    (c.position == CHAT_POS_SHITA && c.line >= it->line || c.position == CHAT_POS_UE && c.line <= it->line))
					{
						// �d�Ȃ�R�����g�̕\���������k�߂�
						if (c.line == it->line && DWORD_DIFF(c.pts, it->pts) <= displayDuration_) {
							it->pts = c.pts - displayDuration_;
						}
						break;
					}
				}
			}
			chatList_.insert(it, c);
		}

		// End CBlockLock
		}

		if (currentWindowWidth_ != width) {
			currentWindowWidth_ = -1;
			textureList_.clear();
		}
		SetRect(prcUnused, 0, 0, width, height);
		SetRect(prcUnusedWoShita, 0, 0, width, height);
		*pbHasFirstDrawShita = false;

		Gdiplus::REAL shadowOffset = (Gdiplus::REAL)(fontScale_ * height / actLineCount / 15);
		if (fontOutline_) {
			// �����\������300%����ɐݒ�ɉ����đ���
			shadowOffset = shadowOffset * fontOutline_ / 150;
		}
		if (!bAntiAlias_) {
			// �����_�ȉ��̂���͕`�挋�ʂ��������ߎl�̌ܓ�
			shadowOffset = (Gdiplus::REAL)max((int)(shadowOffset + 0.5), 1);
		}
		Gdiplus::GraphicsPath grPath;

		std::list<CHAT>::iterator it = chatList_.begin();
		for (; it != chatList_.end(); ++it) {
			const Gdiplus::Font *pFont = it->bMultiLine ? &(it->bSmall ? fontMultiSmall : fontMulti) : &(it->bSmall ? fontSmall : font);
			if (currentWindowWidth_ < 0) {
				// ���ۂ̕`��T�C�Y���v��
				Gdiplus::RectF rcDraw;
				if (g.MeasureString(it->text, -1, pFont, Gdiplus::PointF(0, 0), &rcDraw) == Gdiplus::Ok) {
					it->currentDrawWidth = (int)rcDraw.Width;
					it->currentDrawHeight = (int)rcDraw.Height;
				}
			}
			// �܂��\���^�C�~���O�ɒB���Ȃ��R�����g�͖���
			if (DWORD_MSB(rts_ - it->pts)) {
				continue;
			}
			// �E�B���h�E���ɂ͂ݏo���R�����g�͕\�����C���s�ԂɈڂ�
			// ����ł��͂ݏo���R�����g�͖���
			double actLine = it->line >= actLineCount ? (it->line - actLineCount) + 0.5 : it->line;
			if (actLine < 0 || actLineCount <= actLine) {
				continue;
			}
			Gdiplus::FontFamily fontFamily;
			pFont->GetFamily(&fontFamily);
			Gdiplus::Color foreColor(it->color | Gdiplus::Color::AlphaMask);
			Gdiplus::Color shadowColor(3*foreColor.GetR() + 6*foreColor.GetG() + foreColor.GetB() < 255 ? Gdiplus::Color::White : Gdiplus::Color::Black);
			Gdiplus::Color backColor1(Gdiplus::Color(it->color).GetA(), shadowColor.GetR(), shadowColor.GetG(), shadowColor.GetB());
			Gdiplus::Color backColor2(bAntiAlias_ ? it->color : backColor1.GetValue());
			bool bOpaque = backColor1.GetA() != 0;
			int entireDrawWith = it->currentDrawWidth + (int)shadowOffset + 3;
			int entireDrawHeight = it->currentDrawHeight + (int)shadowOffset + 3;

			std::list<TEXTURE>::iterator jt = textureList_.end();
			if (bUseTexture_) {
				bool bCreateTexture = true;
				// �`�捂�����e�N�X�`���̍ő卂���ɔ[�܂�Ȃ��Ȃ�e�N�X�`�������ׂč�蒼��
				if (textureList_.empty() || entireDrawHeight > currentTextureHeight_) {
					// �����s�R�����g�̓e�N�X�`�������Ȃ�
					if (it->bMultiLine) {
						bCreateTexture = false;
					} else {
						currentTextureHeight_ = entireDrawHeight;
						textureList_.clear();
					}
				}
				// �e�N�X�`����T��
				jt = textureList_.begin();
				std::list<TEXTURE>::const_iterator jtMin, kt;
				POINT minPos = {0, 0};
				int minWidth = INT_MAX;
				int gapWidth = (jt != textureList_.end() && jt->rc.top == 0) ? jt->rc.left : textureWidth;
				if (entireDrawWith <= gapWidth && currentTextureHeight_ <= height) {
					minWidth = gapWidth;
					jtMin = jt;
				}
				while (jt != textureList_.end() && !jt->IsMatch(*it)) {
					kt = jt++;
					// �e�N�X�`�������ꍇ�ɂ����Ƃ��K���Ȍ��Ԃ�T��(�x�X�g�t�B�b�g)
					if (jt != textureList_.end() && jt->rc.top == kt->rc.top) {
						gapWidth = jt->rc.left - kt->rc.right;
					} else {
						// ���s�Ɍׂ�Ƃ��͎��s�̐擪���`�F�b�N
						gapWidth = (jt != textureList_.end() && jt->rc.top == kt->rc.top + currentTextureHeight_) ? jt->rc.left : textureWidth;
						if (entireDrawWith <= gapWidth && gapWidth < minWidth && kt->rc.top + currentTextureHeight_ * 2 <= height) {
							minWidth = gapWidth;
							jtMin = jt;
							minPos.x = 0;
							minPos.y = kt->rc.top + currentTextureHeight_;
						}
						gapWidth = textureWidth - kt->rc.right;
					}
					if (entireDrawWith <= gapWidth && gapWidth < minWidth) {
						minWidth = gapWidth;
						jtMin = jt;
						minPos.x = kt->rc.right;
						minPos.y = kt->rc.top;
					}
				}
				if (jt == textureList_.end() && minWidth != INT_MAX && bCreateTexture) {
					// �e�N�X�`����������Ȃ������̂ō��
					TEXTURE t;
					SetRect(&t.rc, minPos.x, minPos.y, minPos.x + entireDrawWith, minPos.y + entireDrawHeight);
					t.color = it->color;
					t.bSmall = it->bSmall;
					lstrcpy(t.text, it->text);
					pgTexture_->SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
					pgTexture_->SetSmoothingMode(Gdiplus::SmoothingModeNone);
					if (bOpaque) {
						Gdiplus::LinearGradientBrush brBack(Gdiplus::Rect(0, t.rc.top - 1, 1, entireDrawHeight + 1),
						                                    backColor2, backColor1, Gdiplus::LinearGradientModeVertical);
						pgTexture_->FillRectangle(&brBack, t.rc.left, t.rc.top, entireDrawWith, entireDrawHeight);
					} else {
						Gdiplus::SolidBrush brBack(Gdiplus::Color::Transparent);
						pgTexture_->FillRectangle(&brBack, t.rc.left, t.rc.top, entireDrawWith, entireDrawHeight);
					}
					Gdiplus::SolidBrush br(foreColor);
					Gdiplus::PointF pt((Gdiplus::REAL)t.rc.left, (Gdiplus::REAL)t.rc.top);
					if (fontOutline_) {
						grPath.Reset();
						grPath.AddString(t.text, -1, &fontFamily, pFont->GetStyle(), pFont->GetSize() / 0.76f,
						                 pt + Gdiplus::PointF(shadowOffset / 2 + 1, shadowOffset / 2 + 1), NULL);
						pgTexture_->SetCompositingMode(bOpaque ? Gdiplus::CompositingModeSourceOver : Gdiplus::CompositingModeSourceCopy);
						pgTexture_->SetSmoothingMode(bAntiAlias_ ? Gdiplus::SmoothingModeHighQuality : Gdiplus::SmoothingModeNone);
						Gdiplus::Pen penShadow(shadowColor, shadowOffset / 2);
						penShadow.SetLineJoin(Gdiplus::LineJoinRound);
						pgTexture_->DrawPath(&penShadow, &grPath);
						pgTexture_->SetCompositingMode(bAntiAlias_ ? Gdiplus::CompositingModeSourceOver : Gdiplus::CompositingModeSourceCopy);
						pgTexture_->FillPath(&br, &grPath);
					} else {
						// TODO: Win8�Ƀo�Ofix�������炱�̂ւ���C������ׂ�
						// SourceCopy�̕����y�����͋C�����邯��Win8���ƕ������󂷂�̂œ��ʈ���
						pgTexture_->SetCompositingMode(bOpaque || !bAntiAlias_ && bWindows8_ ? Gdiplus::CompositingModeSourceOver : Gdiplus::CompositingModeSourceCopy);
						Gdiplus::SolidBrush brShadow(shadowColor);
						// Win7�ɂ�����U+2588�̏�[1�s�N�Z���͂ݏo�����ۂ��݂�ꂽ����+1
						pgTexture_->DrawString(t.text, -1, pFont, pt + Gdiplus::PointF(shadowOffset, shadowOffset + 1), &brShadow);
						pgTexture_->SetCompositingMode(bAntiAlias_ || bWindows8_ ? Gdiplus::CompositingModeSourceOver : Gdiplus::CompositingModeSourceCopy);
						pgTexture_->DrawString(t.text, -1, pFont, pt + Gdiplus::PointF(0, 1), &br);
					}
					jt = textureList_.insert(jtMin, t);
				}
			}
			int px, py = (int)((0.5 + actLine) * height / actLineCount - (pFont->GetHeight(96) + shadowOffset) / 2);
			if (it->position == CHAT_POS_DEFAULT) {
				px = width - (int)(rts_ - it->pts) * (width + it->currentDrawWidth) / displayDuration_;
			} else {
				px = (width - it->currentDrawWidth) * it->alignFactor / 2;
			}
			g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
			g.SetSmoothingMode(Gdiplus::SmoothingModeNone);
			if (jt != textureList_.end()) {
				// �e�N�X�`�����g��
				g.DrawImage(pTextureBitmap_, px, py,
				            jt->rc.left, jt->rc.top, jt->rc.right - jt->rc.left, jt->rc.bottom - jt->rc.top, Gdiplus::UnitPixel);
				jt->bUsed = true;
			} else {
				if (bOpaque) {
					Gdiplus::LinearGradientBrush brBack(Gdiplus::Rect(0, py - 1, 1, entireDrawHeight + 1),
					                                    backColor2, backColor1, Gdiplus::LinearGradientModeVertical);
					g.FillRectangle(&brBack, px, py, entireDrawWith, entireDrawHeight);
				}
				Gdiplus::SolidBrush br(foreColor);
				Gdiplus::PointF pt((Gdiplus::REAL)px, (Gdiplus::REAL)py);
				if (fontOutline_) {
					grPath.Reset();
					grPath.AddString(it->text, -1, &fontFamily, pFont->GetStyle(), pFont->GetSize() / 0.76f,
					                 pt + Gdiplus::PointF(shadowOffset / 2 + 1, shadowOffset / 2 + 1), NULL);
					g.SetCompositingMode(bAntiAlias_ ? Gdiplus::CompositingModeSourceOver : Gdiplus::CompositingModeSourceCopy);
					g.SetSmoothingMode(bAntiAlias_ ? Gdiplus::SmoothingModeHighQuality : Gdiplus::SmoothingModeNone);
					Gdiplus::Pen penShadow(shadowColor, shadowOffset / 2);
					penShadow.SetLineJoin(Gdiplus::LineJoinRound);
					g.DrawPath(&penShadow, &grPath);
					g.FillPath(&br, &grPath);
				} else {
					g.SetCompositingMode(bAntiAlias_ || bWindows8_ ? Gdiplus::CompositingModeSourceOver : Gdiplus::CompositingModeSourceCopy);
					Gdiplus::SolidBrush brShadow(shadowColor);
					g.DrawString(it->text, -1, pFont, pt + Gdiplus::PointF(shadowOffset, shadowOffset + 1), &brShadow);
					g.DrawString(it->text, -1, pFont, pt + Gdiplus::PointF(0, 1), &br);
				}
			}

			// �`�悵�������𖢎g�p��`�������
			int top = py;
			int bottom = top + entireDrawHeight;
			if (bottom - prcUnused->top < prcUnused->bottom - top) {
				prcUnused->top = min(max(bottom, prcUnused->top), prcUnused->bottom);
			} else {
				prcUnused->bottom = max(min(top, prcUnused->bottom), prcUnused->top);
			}
			if (it->position != CHAT_POS_SHITA) {
				if (bottom - prcUnusedWoShita->top < prcUnusedWoShita->bottom - top) {
					prcUnusedWoShita->top = min(max(bottom, prcUnusedWoShita->top), prcUnusedWoShita->bottom);
				} else {
					prcUnusedWoShita->bottom = max(min(top, prcUnusedWoShita->bottom), prcUnusedWoShita->top);
				}
			}

			// ���߂ĕ`�悵�����R��������ꍇ
			if (it->position == CHAT_POS_SHITA && !it->bDrew) {
				*pbHasFirstDrawShita = true;
			}
			it->bDrew = true;
		}
		currentWindowWidth_ = width;

		// �g���Ȃ������e�N�X�`��������
		std::list<TEXTURE>::iterator jt = textureList_.begin();
		while (jt != textureList_.end()) {
			if (!jt->bUsed) {
				jt = textureList_.erase(jt);
			} else {
				(jt++)->bUsed = false;
			}
		}

		if (debugFlags_ & 2 && bUseTexture_) {
			// Debug:�e�N�X�`���p�r�b�g�}�b�v��\��
			if (debugFlags_ & 4) {
				Gdiplus::SolidBrush br(Gdiplus::Color(2, 255, 0, 255));
				pgTexture_->SetCompositingMode(Gdiplus::CompositingModeSourceOver);
				pgTexture_->FillRectangle(&br, 0, 0, textureWidth, height);
			}
			g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
			g.DrawImage(pTextureBitmap_, 0, 0, textureWidth / 4, height / 4);
			prcUnused->top = min(max(height / 4, prcUnused->top), prcUnused->bottom);
			prcUnusedWoShita->top = min(max(height / 4, prcUnusedWoShita->top), prcUnusedWoShita->bottom);
		}
		if (debugFlags_ & 1) {
			// Debug:�t���[�����[�g��\��
			static DWORD s_lastTick, s_count, s_fps;
			DWORD tick = timeGetTime();
			++s_count;
			if (tick - s_lastTick >= 1000) {
				s_lastTick = tick;
				s_fps = s_count;
				s_count = 0;
			}
			TCHAR text[64];
			wsprintf(text, TEXT("%dfps %2dobj(%dopm)"), s_fps, (int)chatList_.size(), (int)chatList_.size() * 60000 / displayDuration_);
			Gdiplus::SolidBrush br(s_count % 2 ? Gdiplus::Color::Lime : Gdiplus::Color::Black);
			g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
			g.DrawString(text, -1, &font, s_count % 2 ? Gdiplus::PointF(0, 0) : Gdiplus::PointF(2, 2), &br);
		}
	}
}

LRESULT CALLBACK CCommentWindow::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static const int TIMER_AUTOHIDE = 1;
	CCommentWindow *pThis;
	switch (uMsg) {
	case WM_CREATE:
		pThis = static_cast<CCommentWindow*>((reinterpret_cast<LPCREATESTRUCT>(lParam))->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
		pThis->hwnd_ = hwnd;
		SetTimer(hwnd, TIMER_AUTOHIDE, 1000, NULL);
		return 0;
	case WM_DESTROY:
		// �g�b�v���x������Ȃ��̂�Destroy()�ȊO���瑼���I�ɔj������邱�Ƃ�����
		pThis = reinterpret_cast<CCommentWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		pThis->hwnd_ = NULL;
		return 0;
	case WM_TIMER:
		if (wParam == TIMER_AUTOHIDE) {
			pThis = reinterpret_cast<CCommentWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			bool bEmpty;
			{
				CBlockLock lock(&pThis->chatLock_);
				bEmpty = pThis->chatList_.empty() && pThis->chatPoolList_.empty();
			}
			// �\�����镨���Ȃ��̂ɃE�B���h�E(�܂���OSD)���\����Ԃ��ǂ���
			if (bEmpty && (pThis->bUseOsd_ && pThis->bShowOsd_ || !pThis->bUseOsd_ && IsWindowVisible(hwnd))) {
				if (++pThis->autoHideCount_ >= AUTOHIDE_DELAY) {
					if (pThis->bUseOsd_) {
						pThis->osdCompositor_.DeleteTexture(0, 0);
						pThis->bShowOsd_ = false;
						pThis->osdCompositor_.UpdateSurface();
					} else {
						ShowWindow(hwnd, SW_HIDE);
					}
				}
			} else {
				pThis->autoHideCount_ = 0;
			}
			return 0;
		}
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
