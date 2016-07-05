#include "../stdafx.h"
#include "NichanChatProviderSettingDialog.h"
#include "../resource.h"
#include "NichanUtils/FuzzyNichanThreadSelector.h"
#include "../NichanParser/NichanParser.h"

namespace NicoJKKakolog
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> NichanChatProviderSettingDialog::cvt_utf8_wchar;

	NichanChatProviderSettingDialog::NichanChatProviderSettingDialog(HINSTANCE hInstance, const std::unordered_map<std::wstring, std::wstring> &boards):
		hInstance(hInstance),boards(boards)
	{
	}


	NichanChatProviderSettingDialog::~NichanChatProviderSettingDialog()
	{
	}

	std::unique_ptr<INichanThreadSelector> NichanChatProviderSettingDialog::Show(HWND owner)
	{
		if (DialogBoxParam(this->hInstance, MAKEINTRESOURCE(IDD_NICHAN), owner, DialogProc, (LPARAM)this) != IDOK)
			return nullptr;

		return std::unique_ptr<INichanThreadSelector>(new FuzzyNichanThreadSelector(cvt_utf8_wchar.to_bytes(this->boardUrl), { cvt_utf8_wchar.to_bytes(this->threadTitle) }));
	}
	
	INT_PTR CALLBACK NichanChatProviderSettingDialog::DialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		wchar_t buffer[1024];
		HWND hBoard = GetDlgItem(hWnd, IDC_BOARD);
		HWND hThread=GetDlgItem(hWnd, IDC_THREAD);

		NichanChatProviderSettingDialog *this_ = (NichanChatProviderSettingDialog *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

		switch (msg)
		{
		case WM_INITDIALOG:
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)lParam);
			this_ = (NichanChatProviderSettingDialog *)lParam;

			for(const auto &pair:this_->boards)
				SendMessage(GetDlgItem(hWnd, IDC_BOARD), CB_ADDSTRING, 0, (LPARAM)pair.first.c_str());
			return TRUE;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDOK:
				SendMessage(hBoard, CB_GETLBTEXT, SendMessage(hBoard, CB_GETCURSEL, 0, 0), (LPARAM)buffer);
				this_->boardUrl = this_->boards[buffer];
				SendMessage(hThread, LB_GETTEXT, SendMessage(hThread, LB_GETCURSEL, 0, 0), (LPARAM)buffer);
				this_->threadTitle = buffer;
				EndDialog(hWnd, IDOK);
				break;
			case IDCANCEL:
				EndDialog(hWnd, IDCANCEL);
				break;
			case IDC_BOARD:
				if (HIWORD(wParam) == CBN_SELCHANGE)
					try
					{
						SendMessage((HWND)lParam, CB_GETLBTEXT, SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0), (LPARAM)buffer);
						SendMessage(hThread, LB_RESETCONTENT, 0, 0);
						for (const auto &thread : Nichan::ParseBoardFromUrl(cvt_utf8_wchar.to_bytes(this_->boards[buffer])).thread)
							SendMessage(hThread, LB_ADDSTRING, 0, (LPARAM)cvt_utf8_wchar.from_bytes(thread.title).c_str());
					}
					catch(...)
					{
						MessageBox(hWnd, _T("スレ一覧のダウンロードに失敗しました。\n板のURLが変わっている可能性があります。"), _T("エラー"), 0);
					}
				break;
			}
			return TRUE;
		case WM_CLOSE:
			EndDialog(hWnd, IDCANCEL);
			return TRUE;
		}

		return FALSE;
	}
}