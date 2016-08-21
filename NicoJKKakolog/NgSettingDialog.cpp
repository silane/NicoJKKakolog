#include "../stdafx.h"
#include "../resource.h"
#include <locale>
#include <codecvt>
#include "NgSettingDialog.h"
#include "ChatModRule\IdNgChatModRule.h"
#include "ChatModRule\WordNgChatModRule.h"
#include "NicoJKKakolog.h"

namespace NicoJKKakolog
{
	NgSettingDialog::NgSettingDialog(HINSTANCE instance,HWND owner, const std::vector < std::pair<std::unique_ptr<ChatModRule>, int>> &modrules) :
		instance(instance), modrules(modrules),owner(owner)
	{
		this->dialog = CreateDialog(instance, MAKEINTRESOURCE(IDD_NG), NULL, DialogProc);
		SetWindowLongPtr(this->dialog, GWLP_USERDATA, (LONG_PTR)this);
		ShowWindow(this->dialog, SW_SHOW);
	}


	NgSettingDialog::~NgSettingDialog()
	{
		DestroyWindow(this->dialog);
		this->dialog = nullptr;
		this->owner = nullptr;
	}

	HWND NgSettingDialog::GetHandle() const
	{
		return this->dialog;
	}

	void NgSettingDialog::Show()
	{
		ShowWindow(this->dialog, SW_SHOW);
	}

	void NgSettingDialog::Hide()
	{
		ShowWindow(this->dialog, SW_HIDE);
	}

	INT_PTR CALLBACK NgSettingDialog::DialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		static std::wstring_convert < std::codecvt_utf8<wchar_t>, wchar_t > utf8_wide_conv;

		NgSettingDialog *this_ = (NgSettingDialog *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		wchar_t text[2048];

		switch (msg)
		{
		case WM_INITDIALOG:
			//GUIのルールリストのコラムを設定
			LVCOLUMN lvcol;
			lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
			lvcol.fmt = LVCFMT_LEFT;
			lvcol.cx = 100;             // 表示位置
			lvcol.pszText = _T("ルール");  // 見出し
			ListView_InsertColumn(GetDlgItem(hWnd, IDC_NGLIST), 0, &lvcol);

			lvcol.cx = 50;             // 表示位置
			lvcol.pszText = _T("適用回数カウンター");  // 見出し
			ListView_InsertColumn(GetDlgItem(hWnd, IDC_NGLIST), 1, &lvcol);

			ListView_SetExtendedListViewStyle(GetDlgItem(hWnd, IDC_NGLIST), ListView_GetExtendedListViewStyle(GetDlgItem(hWnd, IDC_NGLIST)) |LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

			PostMessage(hWnd, WM_MODRULEUPDATE, 0, -1);

			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_NGIDADD:
				GetWindowTextW(GetDlgItem(hWnd, IDC_NGTEXTEDIT), text, sizeof(text) / sizeof(text[0]));
				if(text[0]!=L'\0')
					PostMessage(this_->owner, NicoJKKakolog::WM_ADDCHATMODRULE, (WPARAM)new IdNgChatModRule(utf8_wide_conv.to_bytes(text)),0);
				return TRUE;
			case IDC_NGWORDADD:
				GetWindowTextW(GetDlgItem(hWnd, IDC_NGTEXTEDIT), text, sizeof(text) / sizeof(text[0]));
				if(text[0]!=L'\0')
					PostMessage(this_->owner, NicoJKKakolog::WM_ADDCHATMODRULE, (WPARAM)new WordNgChatModRule(utf8_wide_conv.to_bytes(text)), 0);
				return TRUE;
			case IDC_REMOVENG:
				for (int idx = ListView_GetNextItem(GetDlgItem(hWnd, IDC_NGLIST), -1, LVNI_SELECTED | LVNI_ALL); idx != -1; idx=ListView_GetNextItem(GetDlgItem(hWnd, IDC_NGLIST), idx, LVNI_SELECTED | LVNI_ALL))
				{
					LVITEM item;
					item.mask = TVIF_PARAM;
					item.iItem = idx;
					item.iSubItem = 0;
					ListView_GetItem(GetDlgItem(hWnd, IDC_NGLIST), &item);
					PostMessage(this_->owner, NicoJKKakolog::WM_REMOVECHATMODRULE,(WPARAM) item.lParam, 0);
				}
				return TRUE;
			}
			break;
		case WM_MODRULEUPDATE:
			if (lParam < 0)
			{
				ListView_DeleteAllItems(GetDlgItem(hWnd, IDC_NGLIST));
				for (std::size_t i = 0; i < this_->modrules.size(); i++)
				{
					const auto &rule = this_->modrules.at(i);

					LVITEM item;
					std::wstring text = utf8_wide_conv.from_bytes(rule.first->GetDescription());
					item.mask = LVIF_TEXT | LVIF_PARAM;
					item.pszText = new wchar_t[text.size() + 1];
					text.copy(item.pszText, text.size());
					item.pszText[text.size()] = L'\0';
					item.iItem = (int)i;
					item.iSubItem = 0;
					item.lParam = (LPARAM)rule.first.get();
					ListView_InsertItem(GetDlgItem(hWnd, IDC_NGLIST), &item);
					delete[] item.pszText;

					text = std::to_wstring(rule.second);
					item.mask = LVIF_TEXT;
					item.pszText = new wchar_t[text.size() + 1];
					text.copy(item.pszText, text.size());
					item.pszText[text.size()] = L'\0';
					item.iItem = (int)i;
					item.iSubItem = 1;
					ListView_SetItem(GetDlgItem(hWnd, IDC_NGLIST), &item);
					delete[] item.pszText;
				}
			}
			else if (lParam < this_->modrules.size())
			{
				std::size_t idx = (std::size_t)wParam;
				LVITEM item;
				std::wstring text = std::to_wstring(this_->modrules.at(idx).second);
				item.mask = LVIF_TEXT;
				item.pszText = new wchar_t[text.size() + 1];
				text.copy(item.pszText, text.size());
				item.pszText[text.size()] = L'\0';
				item.iItem = (int)idx;
				item.iSubItem = 1;
				ListView_SetItem(GetDlgItem(hWnd, IDC_NGLIST), &item);
				delete[] item.pszText;
			}
			return TRUE;

		case WM_CLOSE:
			DestroyWindow(this_->dialog);
			this_->dialog = nullptr;
			this_->owner = nullptr;
			return TRUE;
		}
		return FALSE;
	}
}