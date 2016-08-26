#pragma once

#include <vector>
#include <utility>
#include <memory>
#include "ChatModRule/IChatModRule.h"

namespace NicoJKKakolog
{
	class NgSettingDialog
	{
	private:
		HINSTANCE instance;
		HWND dialog;
		HWND owner;

		const std::vector < std::pair<std::unique_ptr<IChatModRule>, int>> &modrules;

		static INT_PTR CALLBACK DialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	public:
		static constexpr int WM_MODRULEUPDATE = WM_APP + 2000;//�`���b�g�C�����X�g���X�V���ꂽ

	public:
		NgSettingDialog(HINSTANCE instance, HWND owner, const std::vector < std::pair<std::unique_ptr<IChatModRule>, int>> &modrules);
		~NgSettingDialog();

		HWND GetHandle() const;

		void Show();
		void Hide();
	};
}