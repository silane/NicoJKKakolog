#include "stdafx.h"
#include "NiconicoJikkyouLogFileStreamChatProviderEntry.h"
#include "NicoJikkyouLogFileStreamChatProvider.h"
#include "Util.h"
#include <locale>

namespace NicoJKKakolog
{
	NiconicoJikkyouLogFileStreamChatProviderEntry::NiconicoJikkyouLogFileStreamChatProviderEntry()
	{
	}


	NiconicoJikkyouLogFileStreamChatProviderEntry::~NiconicoJikkyouLogFileStreamChatProviderEntry()
	{
	}
	std::string NiconicoJikkyouLogFileStreamChatProviderEntry::GetName() const
	{
		return u8"ニコニコ実況ファイル読み込み";
	}
	std::string NiconicoJikkyouLogFileStreamChatProviderEntry::GetDescription() const
	{
		return u8"ニコニコ実況ファイルから表示";
	}
	IChatProvider * NiconicoJikkyouLogFileStreamChatProviderEntry::NewProvider()
	{
		static std::wstring_convert < std::codecvt<WCHAR, char, mbstate_t>, WCHAR > multi_wide_conv;
		TCHAR path[MAX_PATH];
		if (!FileOpenDialog(nullptr, TEXT("実況ログ(*.jkl;*.xml)\0*.jkl;*.xml\0すべてのファイル\0*.*\0"), path, _countof(path)))
			return nullptr;

		std::string pathStr=multi_wide_conv.to_bytes(path);
		return new NicoJikkyouLogFileStreamChatProvider(pathStr);
	}
	void NiconicoJikkyouLogFileStreamChatProviderEntry::DeleteProvider(std::unique_ptr<IChatProvider> provider)
	{
	}
}