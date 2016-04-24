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
		return u8"�j�R�j�R�����t�@�C���ǂݍ���";
	}
	std::string NiconicoJikkyouLogFileStreamChatProviderEntry::GetDescription() const
	{
		return u8"�j�R�j�R�����t�@�C������\��";
	}
	IChatProvider * NiconicoJikkyouLogFileStreamChatProviderEntry::NewProvider()
	{
		static std::wstring_convert < std::codecvt<WCHAR, char, mbstate_t>, WCHAR > multi_wide_conv;
		TCHAR path[MAX_PATH];
		if (!FileOpenDialog(nullptr, TEXT("�������O(*.jkl;*.xml)\0*.jkl;*.xml\0���ׂẴt�@�C��\0*.*\0"), path, _countof(path)))
			return nullptr;

		std::string pathStr=multi_wide_conv.to_bytes(path);
		return new NicoJikkyouLogFileStreamChatProvider(pathStr);
	}
	void NiconicoJikkyouLogFileStreamChatProviderEntry::DeleteProvider(std::unique_ptr<IChatProvider> provider)
	{
	}
}