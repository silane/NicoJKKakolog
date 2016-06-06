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
				serviceId = serviceId >> 16 | serviceId << 16;//�Ȃ����t�ɂȂ��Ă���炵��
				jkIdTable[serviceId] = std::stoi(pair.second);
			}catch(const std::invalid_argument &e){}
		}
	}

	NiconicoJikkyouChatProviderEntry::~NiconicoJikkyouChatProviderEntry()
	{
	}
	std::string NiconicoJikkyouChatProviderEntry::GetName() const
	{
		return u8"�j�R�j�R����";
	}
	std::string NiconicoJikkyouChatProviderEntry::GetDescription() const
	{
		return u8"���݂̃R�����g��\��";
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