#pragma once
#include "IChatProviderEntry.h"
#include "../IniFile.h"
namespace NicoJKKakolog
{
	class NiconicoJikkyouChatProviderEntry :
		public IChatProviderEntry
	{
	private:
		std::unordered_map<uint_least32_t, int> jkIdTable;

	public:
		NiconicoJikkyouChatProviderEntry(Utility::IniFile *iniFile);
		virtual ~NiconicoJikkyouChatProviderEntry() override;

		virtual std::wstring GetName() const override;
		virtual std::wstring GetDescription() const override;
		virtual IChatProvider * NewProvider() override;
		virtual void DeleteProvider(std::unique_ptr<IChatProvider> provider) override;
	};

}