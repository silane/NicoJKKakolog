#include "../../stdafx.h"
#include "NiconicoJikkyouLogFileStreamChatProviderEntry.h"
#include "../ChatProvider/NicoJikkyouLogFileStreamChatProvider.h"
#include "../../Util.h"
#include <locale>

namespace NicoJKKakolog
{
	NiconicoJikkyouLogFileStreamChatProviderEntry::NiconicoJikkyouLogFileStreamChatProviderEntry()
	{
	}


	NiconicoJikkyouLogFileStreamChatProviderEntry::~NiconicoJikkyouLogFileStreamChatProviderEntry()
	{
	}
	std::wstring NiconicoJikkyouLogFileStreamChatProviderEntry::GetName() const
	{
		return L"�j�R�j�R�����t�@�C���ǂݍ���";
	}
	std::wstring NiconicoJikkyouLogFileStreamChatProviderEntry::GetDescription() const
	{
		return L"�j�R�j�R�����t�@�C������\��";
	}
	IChatProvider * NiconicoJikkyouLogFileStreamChatProviderEntry::NewProvider()
	{
		std::wstring_convert<std::codecvt_byname<wchar_t, char, std::mbstate_t>, wchar_t> multi_wide_conv(new std::codecvt_byname<wchar_t, char, std::mbstate_t>("japanese"));
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