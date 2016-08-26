#pragma once
#include <string>
#include <codecvt>
#include "../TVTestPlugin.h"
#include "IniFile.h"
#include "ChatProviderEntry/IChatProviderEntry.h"
#include "ChatModRule/IChatModRule.h"
#include "NgSettingDialog.h"

namespace NicoJKKakolog {
	//NicoJK�N���X�����傷����̂ŕ���
	//�ǉ��̏����̈ꕔ��NicoJK�N���X���ɂ�����
	class NicoJKKakolog
	{
	private:
		//+�R���͑��Ŏg���Ă�
		static constexpr int WM_CHECKSTATECHANGE = WM_APP + 1000;//�`���b�g���I���̃`�F�b�N�{�b�N�X���ύX���ꂽ

	public:
		static constexpr int WM_ADDCHATMODRULE = WM_APP + 1001;//�`���b�g�C�����[����ǉ�
		static constexpr int WM_REMOVECHATMODRULE = WM_APP + 1002;//�`���b�g�C�����[�����폜

	private:
		static constexpr int NGSETTINGDIALOG_UPDATE_TIMER = 10000;//NgSettingDialog�̃`���b�g�C�����[�����X�g�̍X�V������^�C�}�[

		HINSTANCE hInstance;
		TVTest::CTVTestApp *tvtest;
		Utility::IniFile iniFile;

		HWND dialog;
		std::unique_ptr<NgSettingDialog> ngSettingDialog;
		
		std::vector<IChatProviderEntry*> chatProviderEntries;
		std::vector<IChatProvider *> chatProviders;

		std::chrono::milliseconds timelag;

		//�`���b�g�̏C�����[���̃��X�g(NG�����܂�)
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