#pragma once
#include "IChatProviderEntry.h"

namespace NicoJKKakolog
{
	class NiconicoJikkyouLogFileStreamChatProviderEntry :
		public IChatProviderEntry
	{
	public:
		NiconicoJikkyouLogFileStreamChatProviderEntry();
		virtual ~NiconicoJikkyouLogFileStreamChatProviderEntry() override;

		virtual std::string GetName() const override;
		virtual std::string GetDescription() const override;
		virtual IChatProvider * NewProvider() override;
		virtual void DeleteProvider(std::unique_ptr<IChatProvider> provider) override;
	};
}