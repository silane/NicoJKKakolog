#pragma once
#include "IChatProviderEntry.h"
#include "../IniFile.h"

namespace NicoJKKakolog
{
	class NichanChatProviderEntry:public IChatProviderEntry
	{
	private:
		HINSTANCE hInstance;
		Utility::IniFile *iniFile;

	public:
		NichanChatProviderEntry(HINSTANCE hInstance,Utility::IniFile *iniFile);
		virtual ~NichanChatProviderEntry()=default;

		virtual std::wstring GetName() const override;
		virtual std::wstring GetDescription() const override;
		virtual IChatProvider *NewProvider() override;
		virtual void DeleteProvider(std::unique_ptr<IChatProvider> provider);
	};
}