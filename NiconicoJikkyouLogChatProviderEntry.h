#pragma once
#include "IChatProviderEntry.h"
#include "IniFile.h"
#include "NiconicoLoginSession.h"
namespace NicoJKKakolog
{

	class NiconicoJikkyouLogChatProviderEntry :
		public IChatProviderEntry
	{
	private:
		std::unordered_map<uint_least32_t, int> jkIdTable;
		NiconicoLoginSession login;
		Utility::IniFile *iniFile;

	public:
		NiconicoJikkyouLogChatProviderEntry(Utility::IniFile *iniFile);
		virtual ~NiconicoJikkyouLogChatProviderEntry() override;

		virtual std::string GetName() const override;
		virtual std::string GetDescription() const override;
		virtual IChatProvider * NewProvider() override;
		virtual void DeleteProvider(std::unique_ptr<IChatProvider> provider) override;
	};

}