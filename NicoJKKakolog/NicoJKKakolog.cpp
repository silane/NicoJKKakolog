#include "../stdafx.h"
#include "NicoJKKakolog.h"
#include "../resource.h"
#include "ChatProviderEntry/NiconicoJikkyouChatProviderEntry.h"
#include "ChatProviderEntry/NiconicoJikkyouLogChatProviderEntry.h"
#include "ChatProviderEntry/NiconicoJikkyouLogFileStreamChatProviderEntry.h"
#include "ChatProviderEntry\NichanChatProviderEntry.h"
#include "ChatModRule\IdNgChatModRule.h"
#include "ChatModRule\WordNgChatModRule.h"
#include "SimpleArgumentParser.h"
#include <iterator>

namespace NicoJKKakolog {

	std::wstring_convert < std::codecvt_utf8<WCHAR>, WCHAR > NicoJKKakolog::utf8_wide_conv;

	NicoJKKakolog::NicoJKKakolog():
		timelag(std::chrono::milliseconds(0))
		//modrules({ std::pair<std::unique_ptr<ChatModRule>,int>(std::piecewise_construct,{new IdNgChatModRule("")},{0}) })
	{
	}

	void NicoJKKakolog::Init(HINSTANCE hInstance,TVTest::CTVTestApp *tvtest, const std::basic_string<TCHAR> &iniFileName)
	{
		this->hInstance = hInstance;
		this->tvtest = tvtest;
		this->iniFile.SetFilePath(iniFileName);

		INITCOMMONCONTROLSEX initCommonCtrl;
		initCommonCtrl.dwSize = 8;
		initCommonCtrl.dwICC = ICC_LISTVIEW_CLASSES;
		InitCommonControlsEx(&initCommonCtrl);

		//NGリストを設定ファイルから作成
		for (const auto &ng : this->iniFile.GetSectionContent(L"NGList"))
		{
			if (ng.first.size() == 0)
				continue;
			if (ng.first[0] == L'U')
				this->modrules.emplace_back(std::unique_ptr<ChatModRule>(new IdNgChatModRule(utf8_wide_conv.to_bytes(ng.second))), 0);
			else if(ng.first[0]==L'W')
				this->modrules.emplace_back(std::unique_ptr<ChatModRule>(new WordNgChatModRule(utf8_wide_conv.to_bytes(ng.second))), 0);
		}
	}

	void NicoJKKakolog::DialogInit(HWND dialog)
	{
		this->dialog = dialog;

		//ChatProviderEntryを登録
		this->chatProviderEntries.insert(std::end(this->chatProviderEntries), {
			new NiconicoJikkyouChatProviderEntry(&this->iniFile),
			new NiconicoJikkyouLogChatProviderEntry(&this->iniFile),
			new NiconicoJikkyouLogFileStreamChatProviderEntry(),
			new NichanChatProviderEntry(this->hInstance,dialog,&this->iniFile)
		});
		
		//コマンドライン引数処理
		Utility::SimpleArgumentParser argparser;
		//    デフォルトでオンにするチャット元
		std::wstring commentSource=argparser.GetOptionArgument(L"jkchatsrc");
		std::vector<std::wstring> defaultOnProviderNames;
		if (!commentSource.empty())
		{
			std::wstring::size_type start,end;
			for (start=0,end=commentSource.find(L',');end != std::wstring::npos; start=end+1,end = commentSource.find(L',',end+1))
			{
				defaultOnProviderNames.push_back(commentSource.substr(start, end-start));
			}
			defaultOnProviderNames.push_back(commentSource.substr(start));
		}
		//    ダイアログでデフォルトで何を表示するか
		std::wstring defaultView = argparser.GetOptionArgument(L"jkdlgview");
		if (defaultView == L"trend")
		{
			SendDlgItemMessage(dialog, IDC_RADIO_FORCE, BM_SETCHECK, BST_CHECKED, 0);
			SendDlgItemMessage(dialog, IDC_RADIO_LOG, BM_SETCHECK, BST_UNCHECKED, 0);
			SendDlgItemMessage(dialog, IDC_RADIO_CHATSELECT, BM_SETCHECK, BST_UNCHECKED, 0);
			PostMessage(dialog, WM_COMMAND, (WPARAM)IDC_RADIO_FORCE,0);
		}
		else if (defaultView == L"log")
		{
			SendDlgItemMessage(dialog, IDC_RADIO_FORCE, BM_SETCHECK, BST_UNCHECKED, 0);
			SendDlgItemMessage(dialog, IDC_RADIO_LOG, BM_SETCHECK, BST_CHECKED, 0);
			SendDlgItemMessage(dialog, IDC_RADIO_CHATSELECT, BM_SETCHECK, BST_UNCHECKED, 0);
			PostMessage(dialog, WM_COMMAND, (WPARAM)IDC_RADIO_LOG, 0);
		}
		else if (defaultView == L"chatselect")
		{
			SendDlgItemMessage(dialog, IDC_RADIO_FORCE, BM_SETCHECK, BST_UNCHECKED, 0);
			SendDlgItemMessage(dialog, IDC_RADIO_LOG, BM_SETCHECK, BST_UNCHECKED, 0);
			SendDlgItemMessage(dialog, IDC_RADIO_CHATSELECT, BM_SETCHECK, BST_CHECKED, 0);
			PostMessage(dialog, WM_COMMAND, (WPARAM)IDC_RADIO_CHATSELECT, 0);
		}

		chatProviders.resize(chatProviderEntries.size(), nullptr);

		//GUIのチャット元リストのコラムを設定
		LVCOLUMN lvcol;
		lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		lvcol.fmt = LVCFMT_LEFT;
		lvcol.cx = 160;             // 表示位置
		lvcol.pszText = _T("チャット取得元");  // 見出し
		ListView_InsertColumn(GetDlgItem(dialog, IDC_LISTVIEW), 0, &lvcol);

		lvcol.cx = 320;             // 表示位置
		lvcol.pszText = _T("説明");  // 見出し
		ListView_InsertColumn(GetDlgItem(dialog, IDC_LISTVIEW), 1, &lvcol);

		ListView_SetExtendedListViewStyle(GetDlgItem(dialog, IDC_LISTVIEW), ListView_GetExtendedListViewStyle(GetDlgItem(dialog, IDC_LISTVIEW)) | LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

		//GUIのチャット元リストを作成
		for (size_t i = 0; i<chatProviderEntries.size(); i++)
			//for(const auto &entry : chatProviderEntries)
		{
			auto &entry = chatProviderEntries.at(i);
			LVITEM item;

			std::wstring text = entry->GetName();
			item.mask = LVIF_TEXT;
			item.pszText = new wchar_t[text.size() + 1];
			text.copy(item.pszText, text.size());
			item.pszText[text.size()] = L'\0';
			item.iItem = (int)i;
			item.iSubItem = 0;
			ListView_InsertItem(GetDlgItem(dialog, IDC_LISTVIEW), &item);
			delete[] item.pszText;

			text = entry->GetDescription();
			item.mask = LVIF_TEXT;
			item.pszText = new wchar_t[text.size() + 1];
			text.copy(item.pszText, text.size());
			item.pszText[text.size()] = L'\0';
			item.iItem = (int)i;
			item.iSubItem = 1;
			ListView_SetItem(GetDlgItem(dialog, IDC_LISTVIEW), &item);
			delete[] item.pszText;

			if (std::find(std::begin(defaultOnProviderNames), std::end(defaultOnProviderNames), entry->GetName()) != std::end(defaultOnProviderNames))
			{
				ListView_SetCheckState(GetDlgItem(dialog, IDC_LISTVIEW), i, TRUE);
				PostMessage(dialog, WM_CHECKSTATECHANGE, 0, (LPARAM)i);
			}
		}

		ShowWindow(GetDlgItem(dialog, IDC_LISTVIEW), SW_HIDE);

		SendMessage(dialog, WM_SIZE, 0, 0);
	}

	INT_PTR NicoJKKakolog::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_CHAT_FORWARD:
				timelag += std::chrono::milliseconds(500);
				return TRUE;
			case IDC_CHAT_BACKWARD:
				timelag -= std::chrono::milliseconds(500);
				return TRUE;
			case IDC_CHAT_SYNC:
				timelag = std::chrono::milliseconds(0);
				return TRUE;
			case IDC_NG:
				ngSettingDialog.reset(new NgSettingDialog(this->hInstance, this->dialog, this->modrules));
				SetTimer(dialog, NGSETTINGDIALOG_UPDATE_TIMER, 8000, NULL);
				return TRUE;
			}
			break;
		case WM_NOTIFY:
			if (((LPNMHDR)lParam)->code == NM_CLICK || ((LPNMHDR)lParam)->code == NM_RCLICK || ((LPNMHDR)lParam)->code == NM_DBLCLK) {
				LVHITTESTINFO ht;

				GetCursorPos(&ht.pt);
				ScreenToClient(((LPNMHDR)lParam)->hwndFrom, &ht.pt);

				ListView_HitTest(((LPNMHDR)lParam)->hwndFrom, &ht);
				if (ht.flags & LVHT_ONITEMSTATEICON)
					PostMessage(dialog, WM_CHECKSTATECHANGE, 0, (LPARAM)ht.iItem);
			}
			return FALSE;
		//listviewのチェックボックスの状態が変わった（アプリケーション定義）
		case WM_CHECKSTATECHANGE:
			{
				int    i = (int)lParam;
				/*TCHAR  szName[256];
				LVITEM item;

				item.mask = LVIF_TEXT;
				item.iItem = i;
				item.iSubItem = 0;
				item.pszText = szName;
				item.cchTextMax = sizeof(szName) / sizeof(TCHAR);
				ListView_GetItem(GetDlgItem(dialog, IDC_LISTVIEW), &item);*/

				if (ListView_GetCheckState(GetDlgItem(dialog, IDC_LISTVIEW), i)) {
					if (chatProviders.at(i) == nullptr)
					{
						IChatProvider *provider = chatProviderEntries.at(i)->NewProvider();
						if (provider)
							chatProviders.at(i) = provider;
						else
							ListView_SetCheckState(GetDlgItem(dialog, IDC_LISTVIEW), i, false);
					}
				}
				else {
					//ListView_DeleteItem(listview, i);
					delete chatProviders.at(i);
					chatProviders.at(i) = nullptr;
				}
			}
			return FALSE;

		case WM_ADDCHATMODRULE:
			this->modrules.emplace_back(std::unique_ptr<ChatModRule>((ChatModRule *) wParam), 0);
			PostMessage(this->ngSettingDialog->GetHandle(), NgSettingDialog::WM_MODRULEUPDATE, 0, -1);
			return FALSE;

		case WM_REMOVECHATMODRULE:
			this->modrules.erase(std::find_if(std::begin(this->modrules), std::end(this->modrules),
				[&wParam](const decltype(this->modrules)::value_type &val) {return val.first.get() == (ChatModRule *)wParam; }));
			PostMessage(this->ngSettingDialog->GetHandle(), NgSettingDialog::WM_MODRULEUPDATE, 0, -1);
			return FALSE;

		case WM_TIMER:
			if(wParam== NGSETTINGDIALOG_UPDATE_TIMER)
				PostMessage(this->ngSettingDialog->GetHandle(), NgSettingDialog::WM_MODRULEUPDATE, 0, -1);
			return FALSE;
		}

		return FALSE;
	}

	std::vector<Chat> NicoJKKakolog::GetChats(std::time_t t)
	{
		TVTest::ProgramInfo pi;
		TVTest::ServiceInfo si;
		TVTest::ChannelInfo ci;
		std::memset(&pi, 0, sizeof(pi));
		std::memset(&si, 0, sizeof(si));

		int svcIdx = tvtest->GetService();

		tvtest->GetCurrentProgramInfo(&pi);
		if (svcIdx != -1)
			tvtest->GetServiceInfo(svcIdx, &si);
		tvtest->GetCurrentChannelInfo(&ci);

		ChannelInfo chin;
		chin.NetworkId = ci.NetworkID;
		chin.TransportstreamId = ci.TransportStreamID;
		chin.ServiceId = si.ServiceID;
		chin.EventId = pi.EventID;
		chin.NetworkName.append(ci.szNetworkName);
		chin.TransportstreamName.append(ci.szTransportStreamName);
		chin.ServiceName.append(si.szServiceName);
		if (pi.pszEventName != 0)
			chin.EventName.append(pi.pszEventName);
		if (pi.MaxEventText != 0)
			chin.EventText.append(pi.pszEventText);
		if (pi.pszEventExtText != 0)
			chin.EventExtText.append(pi.pszEventExtText);
		chin.PhysicalChannel = ci.PhysicalChannel;
		FILETIME fiti;
		SystemTimeToFileTime(&pi.StartTime, &fiti);
		chin.StartTime = std::chrono::system_clock::from_time_t(FileTimeToUnixTime(fiti));
		chin.Duration = std::chrono::seconds(pi.Duration);

		auto tp = std::chrono::system_clock::from_time_t(t)+timelag;
		std::vector<Chat> ret;
		int i = 0;
		for (IChatProvider *&provider : chatProviders)
		{
			if (provider != nullptr)
			{
				try
				{
					auto hoge = provider->Get(chin, tp);
					for (Chat &chat : hoge)
					{
						//チャット修正
						for (auto &modrule : this->modrules)
						{
							if (modrule.first->Modify(chat))
								modrule.second++;
						}

						ret.push_back(std::move(chat));
					}
				}
				catch (const ChatProviderError &e) {
					MessageBoxA(this->dialog, e.what(), "チャット取得エラー", 0);
					//TODO: ChatProviderとChatProviderEntryが１対１に対応している前提のコードになっちゃってる
					ListView_SetCheckState(GetDlgItem(dialog, IDC_LISTVIEW), i, FALSE);
					SendMessage(dialog, WM_CHECKSTATECHANGE, 0, (LPARAM)i);
				}
			}
			i++;
		}

		return ret;
	}

	NicoJKKakolog::~NicoJKKakolog()
	{
		for (IChatProvider *provider : this->chatProviders)
		{
			delete provider;
		}

		for (IChatProviderEntry *entry : this->chatProviderEntries)
		{
			delete entry;
		}
	}



	std::time_t NicoJKKakolog::FileTimeToUnixTime(const FILETIME &ft)
	{
		LONGLONG ll = (static_cast<LONGLONG>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
		return static_cast<std::time_t>((ll - 116444736000000000) / 10000000);
	}
}