#pragma once
#include <string>
#include <codecvt>
#include "../TVTestPlugin.h"
#include "IniFile.h"
#include "ChatProviderEntry\IChatProviderEntry.h"

namespace NicoJKKakolog {
	//NicoJKクラスが巨大すぎるので分離
	//追加の処理の一部はNicoJKクラス内にもある
	class NicoJKKakolog
	{
	private:
		//+３桁は他で使われてる
		static constexpr int WM_CHECKSTATECHANGE = WM_APP + 1000;//チャット元選択のチェックボックスが変更された

		HINSTANCE hInstance;
		TVTest::CTVTestApp *tvtest;
		Utility::IniFile iniFile;

		//HWND listview;
		HWND dialog;
		
		std::vector<IChatProviderEntry*> chatProviderEntries;
		std::vector<IChatProvider *> chatProviders;

		std::chrono::milliseconds timelag;

	private:
		static std::time_t FileTimeToUnixTime(const FILETIME &ft);

	public:
		static std::wstring_convert < std::codecvt_utf8<WCHAR>, WCHAR > utf8_wide_conv;
		bool showingListView = false;

	public:
		NicoJKKakolog();
		void Init(HINSTANCE hInstance,TVTest::CTVTestApp *tvtest, const std::basic_string<TCHAR> &iniFileName);
		void DialogInit(HWND dialog);
		INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
		std::vector<Chat> GetChats(std::time_t);
		~NicoJKKakolog();
	};

}