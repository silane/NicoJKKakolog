#pragma once
#include "IChatModRule.h"

namespace NicoJKKakolog
{
	class JyougeIroKomeNgChatModRule :public IChatModRule
	{
		int i;
	public:
		virtual bool Modify(Chat &chat) override;
		virtual std::string GetDescription() const override;
	};
}