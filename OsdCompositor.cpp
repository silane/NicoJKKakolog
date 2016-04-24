#include "stdafx.h"
#include <DbgHelp.h>
#include "OsdCompositor.h"
#include <vmr9.h>
#include <evr9.h>

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "mfuuid.lib")

#if 0
static int GetRefCount(IUnknown *pUnk)
{
    if (!pUnk) return 0;
    pUnk->AddRef();
    return pUnk->Release();
}
#endif

#define WM_SET_CONTAINER_WINDOW (WM_APP + 0)
#define WM_ADD_TEXTURE          (WM_APP + 1)
#define WM_DELETE_TEXTURE       (WM_APP + 2)
#define WM_GET_TEXTURE_INFO     (WM_APP + 3)
#define WM_SET_TEXTURE_INFO     (WM_APP + 4)
#define WM_GET_SURFACE_RECT     (WM_APP + 5)
#define WM_UPDATE_SURFACE       (WM_APP + 6)
#if OSD_COMPOSITOR_VERSION >= 1
#define WM_SET_UPDATE_CALLBACK  (WM_APP + 7)
#define WM_GET_VERSION          (WM_APP + 8)
#endif

static const LPCTSTR OSD_COMPOSITOR_WINDOW_CLASS = TEXT("TVTest Plugin OsdCompositor");
static const CLSID CLSID_madVR = {0xe1a8b82a, 0x32ce, 0x4b0d, {0xbe, 0x0d, 0xaa, 0x68, 0xc7, 0x72, 0xe4, 0x23}};

COsdCompositor::COsdCompositor()
    : m_hwnd(NULL)
    , m_hwndContainer(NULL)
    , m_hOle32(NULL)
    , m_pfnCoCreateInstance(NULL)
    , m_pRenderer(NULL)
    , m_RendererType(RT_VMR9)
    , m_hD3D9(NULL)
    , m_pD3D9(NULL)
    , m_pD3DD9(NULL)
    , m_pD3DS9(NULL)
    , m_TxListLen(0)
    , m_TxCount(0)
    , m_GroupListLen(0)
#if OSD_COMPOSITOR_VERSION >= 1
    , m_CallbackListLen(0)
    , m_LocCallback(NULL)
#endif
{
}

COsdCompositor::~COsdCompositor()
{
    Uninitialize();
}

HWND COsdCompositor::FindHandle()
{
    HWND hwnd = NULL;
    while ((hwnd = ::FindWindowEx(HWND_MESSAGE, hwnd, OSD_COMPOSITOR_WINDOW_CLASS, NULL)) != NULL) {
        DWORD pid;
        ::GetWindowThreadProcessId(hwnd, &pid);
        if (pid == ::GetCurrentProcessId()) break;
    }
    return hwnd;
}

LRESULT COsdCompositor::SendMessageToHandle(UINT Msg, WPARAM wParam, LPARAM lParam) const
{
    // �����ŃE�B���h�E���쐬�����ꍇ�͒T���Ȃ��Ă���
    HWND hwnd = m_hwnd ? m_hwnd : FindHandle();
    if (hwnd) {
        return ::SendMessage(hwnd, Msg, wParam, lParam);
    }
    return FALSE;
}

// TVTest��"Video Container"�E�B���h�E�̃n���h����ݒ肷��
// ���̃E�B���h�E�͋N�����ɕω����Ȃ��悤�Ȃ̂ŁA�I�u�W�F�N�g�𗘗p���鏉���1�x�����Ăׂ΂���
bool COsdCompositor::SetContainerWindow(HWND hwndContainer)
{
    return SendMessageToHandle(WM_SET_CONTAINER_WINDOW, 0, reinterpret_cast<LPARAM>(hwndContainer)) != FALSE;
}

// OSD�̃e�N�X�`����ǉ�����
// Group��0�ȊO�̒l�����Ă�����DeleteTexture()�ł܂Ƃ߂č폜�ł���
// �e�N�X�`����ID�A�܂���0(���s)��Ԃ�
int COsdCompositor::AddTexture(HBITMAP hbm, int Left, int Top, bool fShow, int Group)
{
    if (m_GroupListLen < _countof(m_GroupList)) {
        TEXTURE_PARAM txp = {0};
        txp.nSize = sizeof(TEXTURE_PARAM);
        txp.Left = !fShow ? Left - 200000 : Left;
        txp.Top = Top;
        txp.hbm = hbm;
        int ID = static_cast<int>(SendMessageToHandle(WM_ADD_TEXTURE, 0, reinterpret_cast<LPARAM>(&txp)));
        if (ID != 0) {
            m_GroupList[m_GroupListLen].ID = ID;
            m_GroupList[m_GroupListLen++].Group = Group;
        }
        return ID;
    }
    return 0;
}

// OSD�̃e�N�X�`�����폜����
// (ID,Group) = (0,0):���ׂč폜, (0,!0):�O���[�v�̃e�N�X�`�����폜, (!0,any):ID�̃e�N�X�`�����폜
// 1�ȏ�̍폜���s�����true��Ԃ�
bool COsdCompositor::DeleteTexture(int ID, int Group)
{
    bool fDeleted = false;
    for (int i = 0; i < m_GroupListLen; ++i) {
        if (!ID && !Group || !ID && m_GroupList[i].Group == Group || m_GroupList[i].ID == ID) {
            if (SendMessageToHandle(WM_DELETE_TEXTURE, 0, m_GroupList[i].ID) != FALSE) {
                fDeleted = true;
            }
            ::memmove(&m_GroupList[i], &m_GroupList[i + 1], (m_GroupListLen - (i + 1)) * sizeof(m_GroupList[0]));
            --m_GroupListLen;
            --i;
        }
    }
    return fDeleted;
}

// �e�N�X�`���̕\��/��\����ݒ肷��
bool COsdCompositor::ShowTexture(bool fShow, int ID, int Group)
{
    bool fModified = false;
    for (int i = 0; i < m_GroupListLen; ++i) {
        if (!ID && !Group || !ID && m_GroupList[i].Group == Group || m_GroupList[i].ID == ID) {
            TEXTURE_PARAM txp = {0};
            txp.nSize = sizeof(TEXTURE_PARAM);
            txp.ID = m_GroupList[i].ID;
            if (SendMessageToHandle(WM_GET_TEXTURE_INFO, 0, reinterpret_cast<LPARAM>(&txp)) != FALSE) {
                TEXTURE_PARAM txq = {0};
                txq.nSize = sizeof(TEXTURE_PARAM);
                txq.ID = txp.ID;
                txq.Left = !fShow && txp.Left >= -100000 ? txp.Left - 200000 :
                           fShow && txp.Left < -100000 ? txp.Left + 200000 : txp.Left;
                txq.Top = txp.Top;
                if (txq.Left != txp.Left) {
                    if (SendMessageToHandle(WM_SET_TEXTURE_INFO, 0, reinterpret_cast<LPARAM>(&txq)) != FALSE) {
                        fModified = true;
                    }
                }
            }
        }
    }
    return fModified;
}

// �`��̈�̋�`�𓾂�
bool COsdCompositor::GetSurfaceRect(RECT *pRect)
{
    return SendMessageToHandle(WM_GET_SURFACE_RECT, 0, reinterpret_cast<LPARAM>(pRect)) != FALSE;
}

// �e�N�X�`�����������ĕ`��̈���X�V����
// AddTexture()/DeleteTexture()������������Ă�
bool COsdCompositor::UpdateSurface()
{
    return SendMessageToHandle(WM_UPDATE_SURFACE, 0, 0) != FALSE;
}

#if OSD_COMPOSITOR_VERSION >= 1
// �T�[�t�F�C�X������(UpdateSurface()���Ă񂾂Ƃ�)�̃R�[���o�b�N��ݒ肷��
// AddTexture()����R�X�g�����ǎ��͂ōĕ`�悪�K�v
bool COsdCompositor::SetUpdateCallback(UpdateCallbackFunc Callback, void *pClientData, bool fTop)
{
    // ���I�u�W�F�N�g���ݒ肵���R�[���o�b�N������Ή���
    if (m_LocCallback) {
        SET_UPDATE_CALLBACK_PARAM ucp = {0};
        ucp.nSize = sizeof(ucp);
        ucp.Flags = 0;
        ucp.Callback = m_LocCallback;
        SendMessageToHandle(WM_SET_UPDATE_CALLBACK, 0, reinterpret_cast<LPARAM>(&ucp));
        m_LocCallback = NULL;
    }
    if (Callback) {
        SET_UPDATE_CALLBACK_PARAM ucp = {0};
        ucp.nSize = sizeof(ucp);
        ucp.Flags = fTop ? 3 : 1;
        ucp.Callback = Callback;
        ucp.pClientData = pClientData;
        if (!SendMessageToHandle(WM_SET_UPDATE_CALLBACK, 0, reinterpret_cast<LPARAM>(&ucp))) {
            return false;
        }
        m_LocCallback = Callback;
    }
    return true;
}

// �������ꂽOsdCompositor�E�B���h�E�̃o�[�W�������擾����
// ���s�܂��͖���ł̏ꍇ��0��Ԃ�
int COsdCompositor::GetVersion()
{
    return static_cast<int>(SendMessageToHandle(WM_GET_VERSION, 0, 0));
}
#endif

// ����������
// ��s�v���O�C�������ɃE�B���h�E���쐬���Ă����ꍇ�ɂ�false��Ԃ��̂ŁA
// ���ۂɃI�u�W�F�N�g�𗘗p�ł��邩�ǂ�����FindHandle()��NULL��Ԃ����ǂ����Ŕ��f����
bool COsdCompositor::Initialize()
{
    if (m_hwnd) return true;

    // �E�B���h�E�N���X�o�^(�v���Z�X���ł���1�Ɍ���)
    HMODULE hBase = ::GetModuleHandle(NULL);
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hBase;
    wc.lpszClassName = OSD_COMPOSITOR_WINDOW_CLASS;
    if (::RegisterClass(&wc)) {
        // �E�B���h�E�쐬
        m_hwnd = ::CreateWindow(OSD_COMPOSITOR_WINDOW_CLASS, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hBase, this);
        if (m_hwnd) {
            // CoCreateInstance���t�b�N
            m_hOle32 = ::LoadLibrary(TEXT("ole32.dll"));
            if (m_hOle32) {
                m_pfnCoCreateInstance = (CoCreateInstanceFunc*)::GetProcAddress(m_hOle32, "CoCreateInstance");
                if (m_pfnCoCreateInstance) {
                    if (Hook(CoCreateInstanceHook, m_pfnCoCreateInstance)) {
                        return true;
                    }
                    m_pfnCoCreateInstance = NULL;
                }
                ::FreeLibrary(m_hOle32);
                m_hOle32 = NULL;
            }
            ::DestroyWindow(m_hwnd);
            m_hwnd = NULL;
        }
        ::UnregisterClass(OSD_COMPOSITOR_WINDOW_CLASS, hBase);
    }
    return false;
}

// �j������
// Initialize()�̖߂�l�Ɋւ�炸�Ă�
void COsdCompositor::Uninitialize()
{
#if OSD_COMPOSITOR_VERSION >= 1
    SetUpdateCallback(NULL);
#endif

    if (m_pfnCoCreateInstance) {
        Hook(m_pfnCoCreateInstance, CoCreateInstanceHook);
        m_pfnCoCreateInstance = NULL;
    }
    if (m_pRenderer) {
        m_pRenderer->Release();
        m_pRenderer = NULL;
    }
    if (m_hOle32) {
        ::FreeLibrary(m_hOle32);
        m_hOle32 = NULL;
    }
    if (m_hwnd) {
        ::DestroyWindow(m_hwnd);
        ::UnregisterClass(OSD_COMPOSITOR_WINDOW_CLASS, ::GetModuleHandle(NULL));
        m_hwnd = NULL;
    }
}

bool COsdCompositor::Hook(CoCreateInstanceFunc *pfnNew, CoCreateInstanceFunc *pfnOld)
{
    HMODULE hBase = ::GetModuleHandle(NULL);
    ULONG ulSize;
    PIMAGE_IMPORT_DESCRIPTOR pIID =
        (PIMAGE_IMPORT_DESCRIPTOR)::ImageDirectoryEntryToData(hBase, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);
    if (pIID) {
        for (; pIID->Name && ::lstrcmpiA("ole32.dll", (char*)hBase + pIID->Name); ++pIID);
        if (pIID->Name) {
            PIMAGE_THUNK_DATA pITD = (PIMAGE_THUNK_DATA)((BYTE*)hBase + pIID->FirstThunk);
            for (; pITD->u1.Function && (DWORD_PTR)pITD->u1.Function != (DWORD_PTR)pfnOld; ++pITD);
            if (pITD->u1.Function) {
                DWORD dwOldProtect;
                if (::VirtualProtect(&pITD->u1.Function, sizeof(DWORD_PTR), PAGE_EXECUTE_READWRITE, &dwOldProtect)) {
                    DWORD_PTR dwpBuf = (DWORD_PTR)pfnNew;
                    ::WriteProcessMemory(::GetCurrentProcess(), &pITD->u1.Function, &dwpBuf, sizeof(DWORD_PTR), NULL);
                    ::VirtualProtect(&pITD->u1.Function, sizeof(DWORD_PTR), dwOldProtect, &dwOldProtect);
                    return true;
                }
            }
        }
    }
    return false;
}

LRESULT CALLBACK COsdCompositor::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    COsdCompositor *pThis = reinterpret_cast<COsdCompositor*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (uMsg) {
    case WM_CREATE:
        {
            LPCREATESTRUCT pcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
            pThis = reinterpret_cast<COsdCompositor*>(pcs->lpCreateParams);
            ::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
        }
        return 0;
    case WM_DESTROY:
        pThis->ReleaseDevice();
        while (pThis->m_TxListLen > 0) {
            _aligned_free(pThis->m_TxList[--pThis->m_TxListLen].pBits);
        }
#if OSD_COMPOSITOR_VERSION >= 1
        pThis->m_CallbackListLen = 0;
#endif
        return 0;
    case WM_SET_CONTAINER_WINDOW:
        {
            HWND hwnd = reinterpret_cast<HWND>(lParam);
            if (pThis->m_hwndContainer != hwnd) {
                pThis->m_hwndContainer = hwnd;
                pThis->ReleaseDevice();
            }
        }
        return TRUE;
    case WM_ADD_TEXTURE:
        if (pThis->m_TxListLen < _countof(pThis->m_TxList)) {
            TEXTURE_PARAM *pTxp = reinterpret_cast<TEXTURE_PARAM*>(lParam);
            BITMAP bm;
            if (::GetObject(pTxp->hbm, sizeof(BITMAP), &bm) &&
                bm.bmType == 0 &&
                bm.bmPlanes == 1 &&
                bm.bmBitsPixel == 32 &&
                bm.bmWidth > 0 &&
                bm.bmHeight > 0)
            {
                // 32bit(�A���t�@�l��)�r�b�g�}�b�v�̂ݎ󂯓����
                TEXTURE tx;
                tx.pBits = _aligned_malloc(bm.bmWidth * bm.bmHeight * 4, 16);
                if (tx.pBits) {
                    // �o�^
                    tx.ID = ++pThis->m_TxCount;
                    tx.Left = pTxp->Left;
                    tx.Top = pTxp->Top;
                    ::memcpy(tx.pBits, bm.bmBits, bm.bmWidth * bm.bmHeight * 4);
                    tx.Width = bm.bmWidth;
                    tx.Height = bm.bmHeight;
                    pThis->m_TxList[pThis->m_TxListLen++] = tx;
                    return tx.ID;
                }
            }
        }
        return 0;
    case WM_DELETE_TEXTURE:
        {
            int ID = static_cast<int>(lParam);
            for (int i = 0; i < pThis->m_TxListLen; ++i) {
                if (pThis->m_TxList[i].ID == ID) {
                    // �o�^����
                    _aligned_free(pThis->m_TxList[i].pBits);
                    ::memmove(&pThis->m_TxList[i], &pThis->m_TxList[i + 1], (pThis->m_TxListLen - (i + 1)) * sizeof(TEXTURE));
                    --pThis->m_TxListLen;
                    return TRUE;
                }
            }
        }
        return FALSE;
    case WM_GET_TEXTURE_INFO:
    case WM_SET_TEXTURE_INFO:
        {
            TEXTURE_PARAM *pTxp = reinterpret_cast<TEXTURE_PARAM*>(lParam);
            for (int i = 0; i < pThis->m_TxListLen; ++i) {
                TEXTURE *p = &pThis->m_TxList[i];
                if (p->ID == pTxp->ID) {
                    if (uMsg == WM_GET_TEXTURE_INFO) {
                        pTxp->nSize = sizeof(TEXTURE_PARAM);
                        pTxp->Left = p->Left;
                        pTxp->Top = p->Top;
                    }
                    else {
                        if (pTxp->Left != INT_MAX) p->Left = pTxp->Left;
                        if (pTxp->Top != INT_MAX) p->Top = pTxp->Top;
                    }
                    return TRUE;
                }
            }
        }
        return FALSE;
    case WM_GET_SURFACE_RECT:
        if (pThis->m_pRenderer) {
            if (GetVideoPosition(reinterpret_cast<RECT*>(lParam), pThis->m_pRenderer, pThis->m_RendererType)) {
                return TRUE;
            }
        }
        return FALSE;
    case WM_UPDATE_SURFACE:
        if (pThis->m_pRenderer) {
            RECT rc;
            if (pThis->CreateDevice() && GetVideoPosition(&rc, pThis->m_pRenderer, pThis->m_RendererType)) {
                int vw = rc.right - rc.left;
                int vh = rc.bottom - rc.top;
                RECT rcSurface;
                if (vw > 0 && vh > 0 && pThis->SetupSurface(vw, vh, &rcSurface)) {
                    NORMALIZEDRECT nrc;
                    nrc.left = (float)rcSurface.left / vw;
                    nrc.top = (float)rcSurface.top / vh;
                    nrc.right = (float)rcSurface.right / vw;
                    nrc.bottom = (float)rcSurface.bottom / vh;
                    SetAlphaBitmap(pThis->m_pRenderer, pThis->m_RendererType, pThis->m_pD3DS9, &nrc);
                }
                else {
                    SetAlphaBitmap(pThis->m_pRenderer, pThis->m_RendererType, NULL, NULL);
                }
            }
        }
        return TRUE;
#if OSD_COMPOSITOR_VERSION >= 1
    case WM_SET_UPDATE_CALLBACK:
        {
            SET_UPDATE_CALLBACK_PARAM *pUcp = reinterpret_cast<SET_UPDATE_CALLBACK_PARAM*>(lParam);
            if (pUcp->Flags & 1) {
                if (pThis->m_CallbackListLen < _countof(pThis->m_CallbackList)) {
                    // �o�^
                    pThis->m_CallbackList[pThis->m_CallbackListLen].pClientData = pUcp->pClientData;
                    pThis->m_CallbackList[pThis->m_CallbackListLen].fTop = (pUcp->Flags & 2) != 0;
                    pThis->m_CallbackList[pThis->m_CallbackListLen++].Callback = pUcp->Callback;
                    return TRUE;
                }
            }
            else {
                for (int i = 0; i < pThis->m_CallbackListLen; ++i) {
                    if (pThis->m_CallbackList[i].Callback == pUcp->Callback) {
                        // �o�^����
                        ::memmove(&pThis->m_CallbackList[i], &pThis->m_CallbackList[i + 1],
                                  (pThis->m_CallbackListLen - (i + 1)) * sizeof(pThis->m_CallbackList[0]));
                        --pThis->m_CallbackListLen;
                        return TRUE;
                    }
                }
            }
        }
        return FALSE;
    case WM_GET_VERSION:
        return OSD_COMPOSITOR_VERSION;
#endif
    }
    return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// �r�b�g�C���[�W���A���t�@��������
// ������bottom-up��top-down�ϊ�����
void COsdCompositor::ComposeAlpha(DWORD* __restrict pBitsDest, int PitchDest, int WidthDest, int HeightDest,
                                  const DWORD* __restrict pBits, int Pitch, int Width, int Height, int Left, int Top)
{
    int ySrc = min(Height - 1 + Top, Height - 1);
    int yDest = max(Top, 0);
    int yRange = max(Top + Height - HeightDest, 0);
    for (; ySrc >= yRange; --ySrc, ++yDest) {
        int i = ySrc * Pitch + max(-Left, 0);
        int j = yDest * PitchDest + max(Left, 0);
        int r = ySrc * Pitch + min(WidthDest - Left, Width);
        for (;; ++i, ++j) {
            // �قƂ�Ǐd�Ȃ�Ȃ����Ƃ�z��
            for (; i < r && !(pBitsDest[j] & 0xFF000000); ++i, ++j) {
                pBitsDest[j] = pBits[i];
            }
            if (i >= r) break;

            // �d�Ȃ肪����ꍇ
            DWORD v = pBits[i];
            DWORD w = pBitsDest[j];
            BYTE va = v>>24;
            BYTE wa = w>>24;
            BYTE outa = va + ((wa*(255-va)+255)>>8);
            if (outa) {
                BYTE outr = ((v>>16&0xFF)*va + (((w>>16&0xFF)*wa*(255-va)+255)>>8)) / outa;
                BYTE outg = ((v>>8&0xFF)*va + (((w>>8&0xFF)*wa*(255-va)+255)>>8)) / outa;
                BYTE outb = ((v&0xFF)*va + (((w&0xFF)*wa*(255-va)+255)>>8)) / outa;
                pBitsDest[j] = outa<<24 | outr<<16 | outg<<8 | outb;
            }
            else {
                pBitsDest[j] = 0;
            }
        }
    }
}

bool COsdCompositor::GetVideoPosition(RECT *pRect, IBaseFilter *pRenderer, RENDERER_TYPE RendererType)
{
    bool fRet = false;
    if (RendererType == RT_VMR9) {
        IVMRFilterConfig9 *pVMRFC9;
        HRESULT hr = pRenderer->QueryInterface(IID_IVMRFilterConfig9, (void**)&pVMRFC9);
        if (SUCCEEDED(hr)) {
            DWORD dwMode;
            hr = pVMRFC9->GetRenderingMode(&dwMode);
            if (SUCCEEDED(hr) && dwMode == VMR9Mode_Windowless) {
                IVMRWindowlessControl9 *pVMRWC9;
                hr = pRenderer->QueryInterface(IID_IVMRWindowlessControl9, (void**)&pVMRWC9);
                if (SUCCEEDED(hr)) {
                    RECT rcSrc;
                    hr = pVMRWC9->GetVideoPosition(&rcSrc, pRect);
                    if (SUCCEEDED(hr)) {
                        fRet = true;
                    }
                    pVMRWC9->Release();
                }
            }
            pVMRFC9->Release();
        }
    }
    else if (RendererType == RT_EVR) {
        IMFGetService *pMFGS;
        HRESULT hr = pRenderer->QueryInterface(IID_IMFGetService, (void**)&pMFGS);
        if (SUCCEEDED(hr)) {
            IMFVideoDisplayControl *pIMFVDC;
            hr = pMFGS->GetService(MR_VIDEO_RENDER_SERVICE, IID_IMFVideoDisplayControl, (void**)&pIMFVDC);
            if (SUCCEEDED(hr)) {
                MFVideoNormalizedRect nrcSrc;
                hr = pIMFVDC->GetVideoPosition(&nrcSrc, pRect);
                if (SUCCEEDED(hr)) {
                    fRet = true;
                }
                pIMFVDC->Release();
            }
            pMFGS->Release();
        }
    }
    return fRet;
}

bool COsdCompositor::SetAlphaBitmap(IBaseFilter *pRenderer, RENDERER_TYPE RendererType, IDirect3DSurface9 *pD3DS9, const NORMALIZEDRECT *pNrc)
{
    bool fRet = false;
    if (RendererType == RT_VMR9) {
        IVMRMixerBitmap9 *pVMRMB9;
        HRESULT hr = pRenderer->QueryInterface(IID_IVMRMixerBitmap9, (void**)&pVMRMB9);
        if (SUCCEEDED(hr)) {
            VMR9AlphaBitmap ab;
            ab.dwFlags = pD3DS9 ? VMR9AlphaBitmap_EntireDDS | VMR9AlphaBitmap_FilterMode : VMR9AlphaBitmap_Disable;
            ab.hdc = NULL;
            ab.pDDS = pD3DS9;
            ab.rDest.left = pNrc ? pNrc->left : 0.0f;
            ab.rDest.top = pNrc ? pNrc->top : 0.0f;
            ab.rDest.right = pNrc ? pNrc->right : 1.0f;
            ab.rDest.bottom = pNrc ? pNrc->bottom : 1.0f;
            ab.fAlpha = 1.0f;
            ab.dwFilterMode = MixerPref_PointFiltering;
            hr = pVMRMB9->SetAlphaBitmap(&ab);
            if (SUCCEEDED(hr)) {
                fRet = true;
            }
            pVMRMB9->Release();
        }
    }
    else if (RendererType == RT_EVR) {
        IMFGetService *pMFGS;
        HRESULT hr = pRenderer->QueryInterface(IID_IMFGetService, (void**)&pMFGS);
        if (SUCCEEDED(hr)) {
            IMFVideoMixerBitmap *pMFVMB;
            hr = pMFGS->GetService(MR_VIDEO_MIXER_SERVICE, IID_IMFVideoMixerBitmap, (void**)&pMFVMB);
            if (SUCCEEDED(hr)) {
                if (pD3DS9) {
                    MFVideoAlphaBitmap ab;
                    ab.GetBitmapFromDC = FALSE;
                    ab.bitmap.pDDS = pD3DS9;
                    ab.params.dwFlags = MFVideoAlphaBitmap_EntireDDS | MFVideoAlphaBitmap_FilterMode;
                    if (pNrc) {
                        ab.params.dwFlags |= MFVideoAlphaBitmap_DestRect;
                        ab.params.nrcDest.left = pNrc->left;
                        ab.params.nrcDest.top = pNrc->top;
                        ab.params.nrcDest.right = pNrc->right;
                        ab.params.nrcDest.bottom = pNrc->bottom;
                    }
                    // MSDN�ɂ���MFVideoAlphaBitmap_FilterMode�̃f�t�H���g�̂͂��������ۂɂ͈Ⴄ�݂���
                    ab.params.dwFilterMode = D3DTEXF_POINT;
                    hr = pMFVMB->SetAlphaBitmap(&ab);
                    if (SUCCEEDED(hr)) {
                        fRet = true;
                    }
                }
                else {
                    hr = pMFVMB->ClearAlphaBitmap();
                    if (SUCCEEDED(hr)) {
                        fRet = true;
                    }
                }
                pMFVMB->Release();
            }
            pMFGS->Release();
        }
    }
    return fRet;
}

bool COsdCompositor::CreateDevice()
{
    if (m_hD3D9) return true;
    if (!m_hwndContainer) return false;

    m_hD3D9 = ::LoadLibrary(TEXT("d3d9.dll"));
    if (m_hD3D9) {
        Direct3DCreate9Func *pfnDirect3DCreate9 = (Direct3DCreate9Func*)::GetProcAddress(m_hD3D9, "Direct3DCreate9");
        if (pfnDirect3DCreate9) {
            m_pD3D9 = pfnDirect3DCreate9(D3D_SDK_VERSION);
            if (m_pD3D9) {
                D3DDISPLAYMODE dm;
                HRESULT hr = m_pD3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm);
                if (SUCCEEDED(hr)) {
                    D3DPRESENT_PARAMETERS pp = {0};
                    pp.BackBufferFormat = dm.Format;
                    pp.MultiSampleType = D3DMULTISAMPLE_NONE;
                    pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
                    pp.Windowed = TRUE;
                    pp.hDeviceWindow = m_hwndContainer;
                    hr = m_pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL,
                                               D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &m_pD3DD9);
                    if (SUCCEEDED(hr)) {
                        return true;
                    }
                    m_pD3DD9 = NULL;
                }
                m_pD3D9->Release();
                m_pD3D9 = NULL;
            }
        }
        ::FreeLibrary(m_hD3D9);
        m_hD3D9 = NULL;
    }
    return false;
}

void COsdCompositor::ReleaseDevice()
{
    if (m_pD3DS9) {
        m_pD3DS9->Release();
        m_pD3DS9 = NULL;
    }
    if (m_pD3DD9) {
        m_pD3DD9->Release();
        m_pD3DD9 = NULL;
    }
    if (m_pD3D9) {
        m_pD3D9->Release();
        m_pD3D9 = NULL;
    }
    if (m_hD3D9) {
        ::FreeLibrary(m_hD3D9);
        m_hD3D9 = NULL;
    }
}

bool COsdCompositor::SetupSurface(int VideoWidth, int VideoHeight, RECT *pSurfaceRect)
{
    // �e�N�X�`�����f���̈�ɔz�u���邽�߂ɕK�v�ȍŏ���`�����Ƃ߂�
    RECT rcVideo = { 0, 0, VideoWidth, VideoHeight };
    RECT rcSurface = {0};
    for (int i = 0; i < m_TxListLen; ++i) {
        RECT rc = { m_TxList[i].Left, m_TxList[i].Top,
                    m_TxList[i].Left + m_TxList[i].Width,
                    m_TxList[i].Top + m_TxList[i].Height };
        RECT rcInter;
        if (::IntersectRect(&rcInter, &rcVideo, &rc)) {
            // �e�N�X�`���͉f���̈���ɂ���
            RECT rcUnion;
            ::UnionRect(&rcUnion, &rcSurface, &rcInter);
            rcSurface = rcUnion;
        }
    }
    // �T�[�t�F�C�X������
    HRESULT hr;
    if (m_pD3DS9) {
        D3DSURFACE_DESC desc;
        hr = m_pD3DS9->GetDesc(&desc);
        if (FAILED(hr) || ::IsRectEmpty(&rcSurface) ||
            (int)desc.Width != rcSurface.right - rcSurface.left ||
            (int)desc.Height != rcSurface.bottom - rcSurface.top)
        {
            m_pD3DS9->Release();
            m_pD3DS9 = NULL;
        }
    }
    if (!m_pD3DS9 && m_pD3DD9 && !::IsRectEmpty(&rcSurface)) {
        hr = m_pD3DD9->CreateOffscreenPlainSurface(rcSurface.right - rcSurface.left, rcSurface.bottom - rcSurface.top,
                                                   D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &m_pD3DS9, NULL);
        if (FAILED(hr)) {
            m_pD3DS9 = NULL;
        }
    }
    // �e�N�X�`�����T�[�t�F�C�X�ɍ���
    if (m_pD3DS9) {
        D3DLOCKED_RECT lr;
        HRESULT hr = m_pD3DS9->LockRect(&lr, NULL, 0);
        if (SUCCEEDED(hr)) {
            //::memset(lr.pBits, 0x60, lr.Pitch * (rcSurface.bottom - rcSurface.top)); // DEBUG
            ::memset(lr.pBits, 0, lr.Pitch * (rcSurface.bottom - rcSurface.top));
#if OSD_COMPOSITOR_VERSION >= 1
            for (int i = 0; i < m_CallbackListLen; ++i) {
                if (!m_CallbackList[i].fTop) {
                    m_CallbackList[i].Callback(lr.pBits, &rcSurface, lr.Pitch, m_CallbackList[i].pClientData);
                }
            }
#endif
            for (int i = 0; i < m_TxListLen; ++i) {
                ComposeAlpha((DWORD*)lr.pBits, lr.Pitch / 4,
                             rcSurface.right - rcSurface.left, rcSurface.bottom - rcSurface.top,
                             (DWORD*)m_TxList[i].pBits, m_TxList[i].Width, m_TxList[i].Width, m_TxList[i].Height,
                             m_TxList[i].Left - rcSurface.left, m_TxList[i].Top - rcSurface.top);
            }
#if OSD_COMPOSITOR_VERSION >= 1
            for (int i = 0; i < m_CallbackListLen; ++i) {
                if (m_CallbackList[i].fTop) {
                    m_CallbackList[i].Callback(lr.pBits, &rcSurface, lr.Pitch, m_CallbackList[i].pClientData);
                }
            }
#endif
            m_pD3DS9->UnlockRect();
        }
    }
    *pSurfaceRect = rcSurface;
    return m_pD3DS9 != NULL;
}

HRESULT WINAPI COsdCompositor::CoCreateInstanceHook(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID FAR* ppv)
{
    HRESULT hr = E_FAIL;
    HWND hwnd = FindHandle();
    if (hwnd) {
        COsdCompositor *pThis = reinterpret_cast<COsdCompositor*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (pThis) {
            // ���̃����_�������������Ƃ����
            if (pThis->m_pRenderer &&
                (IsEqualCLSID(CLSID_VideoMixingRenderer, rclsid) ||
                 IsEqualCLSID(CLSID_VideoMixingRenderer9, rclsid) ||
                 IsEqualCLSID(CLSID_EnhancedVideoRenderer, rclsid) ||
                 IsEqualCLSID(CLSID_OverlayMixer, rclsid) ||
                 IsEqualCLSID(CLSID_madVR, rclsid)))
            {
                pThis->m_pRenderer->Release();
                pThis->m_pRenderer = NULL;
            }
            hr = pThis->m_pfnCoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);
            // �����_���̐������Ď�(TVTest��VideoRenderer.cpp�Q��)
            // �K�����C���X���b�h�ŌĂ΂�邱�Ƃ����肵�Ă���
            if (SUCCEEDED(hr)) {
                if (IsEqualCLSID(CLSID_VideoMixingRenderer9, rclsid)) {
                    pThis->m_pRenderer = static_cast<IBaseFilter*>(*ppv);
                    pThis->m_pRenderer->AddRef();
                    pThis->m_RendererType = RT_VMR9;
                }
                else if (IsEqualCLSID(CLSID_EnhancedVideoRenderer, rclsid)) {
                    pThis->m_pRenderer = static_cast<IBaseFilter*>(*ppv);
                    pThis->m_pRenderer->AddRef();
                    pThis->m_RendererType = RT_EVR;
                }
            }
        }
    }
    return hr;
}
