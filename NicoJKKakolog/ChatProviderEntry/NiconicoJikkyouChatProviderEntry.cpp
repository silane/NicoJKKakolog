#include "../../stdafx.h"
#include "NiconicoJikkyouChatProviderEntry.h"
#include "../ChatProvider/NiconicoJikkyouChatProvider.h"
#include "../NiconicoUtils/DefaultJkIdTable.h"

namespace NicoJKKakolog
{
	NiconicoJikkyouChatProviderEntry::NiconicoJikkyouChatProviderEntry(Utility::IniFile *iniFile):
		jkIdTable(GetDefaultJkIdTable())
	{
		for (auto &pair : iniFile->GetSectionContent(TEXT("Channels")))
		{
			try {
				uint_least32_t serviceId = (uint_least32_t)std::stol(pair.first, nullptr, 0);
				serviceId = serviceId >> 16 | serviceId << 16;//なぜか逆になっているらしい
				jkIdTable[serviceId] = std::stoi(pair.second);
			}catch(const std::invalid_argument &e){}
		}
	}

	NiconicoJikkyouChatProviderEntry::~NiconicoJikkyouChatProviderEntry()
	{
	}
	std::wstring NiconicoJikkyouChatProviderEntry::GetName() const
	{
		return L"ニコニコ実況";
	}
	std::wstring NiconicoJikkyouChatProviderEntry::GetDescription() const
	{
		return L"現在のコメントを表示";
	}
	IChatProvider * NiconicoJikkyouChatProviderEntry::NewProvider()
	{
		NiconicoJikkyouChatProvider *ret = new NiconicoJikkyouChatProvider(jkIdTable);
		return ret;
	}
	void NiconicoJikkyouChatProviderEntry::DeleteProvider(std::unique_ptr<IChatProvider> provider)
	{
	}
}