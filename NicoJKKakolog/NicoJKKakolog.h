#pragma once
#include <string>
#include <codecvt>
#include "../TVTestPlugin.h"
#include "IniFile.h"
#include "ChatProviderEntry/IChatProviderEntry.h"
#include "ChatModRule/IChatModRule.h"
#include "NgSettingDialog.h"

namespace NicoJKKakolog {
	//NicoJKクラスが巨大すぎるので分離
	//追加の処理の一部はNicoJKクラス内にもある
	class NicoJKKakolog
	{
	private:
		//+３桁は他で使われてる
		static constexpr int WM_CHECKSTATECHANGE = WM_APP + 1000;//チャット元選択のチェックボックスが変更された

	public:
		static constexpr int WM_ADDCHATMODRULE = WM_APP + 1001;//チャット修正ルールを追加
		static constexpr int WM_REMOVECHATMODRULE = WM_APP + 1002;//チャット修正ルールを削除

	private:
		static constexpr int NGSETTINGDIALOG_UPDATE_TIMER = 10000;//NgSettingDialogのチャット修正ルールリストの更新をするタイマー

		HINSTANCE hInstance;
		TVTest::CTVTestApp *tvtest;
		Utility::IniFile iniFile;

		HWND dialog;
		std::unique_ptr<NgSettingDialog> ngSettingDialog;
		
		std::vector<IChatProviderEntry*> chatProviderEntries;
		std::vector<IChatProvider *> chatProviders;

		std::chrono::milliseconds timelag;

		//チャットの修正ルールのリスト(NG化も含む)
		std::vector<std::pair<std::unique_ptr<IChatModRule>, int>> modrules;

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

		//void AddChatModRule(std::unique_ptr<ChatModRule> rule);
		//const decltype(modrules) &GetChatModRuleList();
		//void RemoveChatModRule()

		~NicoJKKakolog();
	};

}