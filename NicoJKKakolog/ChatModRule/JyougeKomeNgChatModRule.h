#pragma once
#include "IChatModRule.h"
namespace NicoJKKakolog
{
	class JyougeKomeNgChatModRule:public IChatModRule
	{
	public:
		virtual bool Modify(Chat &chat) override;
		virtual std::string GetDescription() const override;
	};
}