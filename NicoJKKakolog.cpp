#include "stdafx.h"
#include "NicoJKKakolog.h"
#include "resource.h"

namespace NicoJKKakolog {

	std::wstring_convert < std::codecvt_utf8<WCHAR>, WCHAR > NicoJKKakolog::utf8_wide_conv;

	NicoJKKakolog::NicoJKKakolog():
		timelag(std::chrono::milliseconds(0))
	{
	}

	void NicoJKKakolog::Init(TVTest::CTVTestApp *tvtest, const std::basic_string<TCHAR> &iniFileName)
	{
		this->tvtest = tvtest;
		this->iniFile.SetFilePath(iniFileName);

		//ChatProviderEntryを登録
		this->chatProviderEntries.insert(std::end(this->chatProviderEntries), {
			new NiconicoJikkyouChatProviderEntry(&iniFile),
			new NiconicoJikkyouLogChatProviderEntry(&iniFile),
			new NiconicoJikkyouLogFileStreamChatProviderEntry()
		});
	}

	void NicoJKKakolog::DialogInit(HWND dialog,HWND listview)
	{
		this->dialog = dialog;
		this->listview = listview;

		INITCOMMONCONTROLSEX initCommonCtrl;
		initCommonCtrl.dwSize = 8;
		initCommonCtrl.dwICC = ICC_LISTVIEW_CLASSES;
		InitCommonControlsEx(&initCommonCtrl);

		LVCOLUMN lvcol;
		lvcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
		lvcol.fmt = LVCFMT_LEFT;
		lvcol.cx = 160;             // 表示位置
		lvcol.pszText = _T("チャット取得元");  // 見出し
		ListView_InsertColumn(listview, 0, &lvcol);

		lvcol.cx = 320;             // 表示位置
		lvcol.pszText = _T("説明");  // 見出し
		ListView_InsertColumn(listview, 1, &lvcol);

		ListView_SetExtendedListViewStyle(listview, ListView_GetExtendedListViewStyle(listview) | LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

		//GUIのチャット元リストを作成
		for (size_t i = 0; i<chatProviderEntries.size(); i++)
			//for(const auto &entry : chatProviderEntries)
		{
			auto &entry = chatProviderEntries.at(i);
			LVITEM item;

			std::wstring text = utf8_wide_conv.from_bytes(entry->GetName());
			item.mask = LVIF_TEXT;
			item.pszText = new wchar_t[text.size() + 1];
			text.copy(item.pszText, text.size());
			item.pszText[text.size()] = L'\0';
			item.iItem = i;
			item.iSubItem = 0;
			ListView_InsertItem(listview, &item);
			delete[] item.pszText;

			text = utf8_wide_conv.from_bytes(entry->GetDescription());
			item.mask = LVIF_TEXT;
			item.pszText = new wchar_t[text.size() + 1];
			text.copy(item.pszText, text.size());
			item.pszText[text.size()] = L'\0';
			item.iItem = i;
			item.iSubItem = 1;
			ListView_SetItem(listview, &item);
			delete[] item.pszText;
		}

		chatProviders.resize(chatProviderEntries.size(), nullptr);

		ShowWindow(listview, SW_HIDE);

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
			TCHAR  szName[256];
			TCHAR  szBuf[256];
			LVITEM item;

			item.mask = LVIF_TEXT;
			item.iItem = i;
			item.iSubItem = 0;
			item.pszText = szName;
			item.cchTextMax = sizeof(szName) / sizeof(TCHAR);
			ListView_GetItem(listview, &item);

			if (ListView_GetCheckState(listview, i)) {
				if (chatProviders.at(i) == nullptr)
				{
					IChatProvider *provider = chatProviderEntries.at(i)->NewProvider();
					if (provider)
						chatProviders.at(i) = provider;
					else
						ListView_SetCheckState(listview, i, false);
				}
			}
			else {
				//ListView_DeleteItem(listview, i);
				delete chatProviders.at(i);
				chatProviders.at(i) = nullptr;
			}
		}
		return FALSE;

		}

		return FALSE;
	}

	std::vector<Chat> NicoJKKakolog::GetChats(std::time_t t)
	{
		TVTest::ProgramInfo pi;
		TVTest::ServiceInfo si;
		TVTest::ChannelInfo ci;
		memset(&pi, 0, sizeof(pi));

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

		std::vector<Chat> ret;
		for (IChatProvider *provider : chatProviders)
		{
			if (provider == nullptr)
				continue;

			auto hoge = provider->Get(chin, std::chrono::system_clock::from_time_t(t)+timelag);
			ret.insert(std::end(ret), std::begin(hoge), std::end(hoge));
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