#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <codecvt>
#include <locale>
#include "NichanUtils\INichanThreadSelector.h"

namespace NicoJKKakolog
{
	class NichanChatProviderSettingDialog
	{
	private:
		static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cvt_utf8_wchar;

		HINSTANCE hInstance;
		std::unordered_map<std::wstring, std::wstring> boards;
		std::wstring boardUrl;
		std::wstring threadTitle;

		static INT_PTR CALLBACK DialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	public:
		NichanChatProviderSettingDialog(HINSTANCE hInstance, const std::unordered_map<std::wstring,std::wstring> &boards);
		~NichanChatProviderSettingDialog();

		std::unique_ptr<INichanThreadSelector> Show(HWND owner=nullptr);
	};
}