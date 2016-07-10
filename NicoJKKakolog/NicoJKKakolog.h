#pragma once
#include <string>
#include <codecvt>
#include "../TVTestPlugin.h"
#include "IniFile.h"
#include "ChatProviderEntry\IChatProviderEntry.h"

namespace NicoJKKakolog {
	//NicoJK�N���X�����傷����̂ŕ���
	//�ǉ��̏����͂��ׂĂ������ɏ����Ă���Ƃ����킯�ł͂Ȃ�
	class NicoJKKakolog
	{
	private:
		//+�R���͑��Ŏg���Ă�
		static constexpr int WM_CHECKSTATECHANGE = WM_APP + 1000;//�`���b�g���I���̃`�F�b�N�{�b�N�X���ύX���ꂽ

		HINSTANCE hInstance;
		TVTest::CTVTestApp *tvtest;
		Utility::IniFile iniFile;

		HWND listview;
		HWND dialog;
		
		std::vector<IChatProviderEntry*> chatProviderEntries;
		std::vector<IChatProvider *> chatProviders;

		std::chrono::milliseconds timelag;

	public:
		static std::wstring_convert < std::codecvt_utf8<WCHAR>, WCHAR > utf8_wide_conv;
		bool showingListView = false;

	public:
		NicoJKKakolog();
		void Init(HINSTANCE hInstance,TVTest::CTVTestApp *tvtest, const std::basic_string<TCHAR> &iniFileName);
		void DialogInit(HWND dialog,HWND listview);
		INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
		std::vector<Chat> GetChats(std::time_t);
		~NicoJKKakolog();

		static std::time_t NicoJKKakolog::FileTimeToUnixTime(const FILETIME &ft);
	};

}