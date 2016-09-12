#pragma once
#include "IChatModRule.h"

namespace NicoJKKakolog
{
	class IroKomeNgChatModRule :
		public IChatModRule
	{
	public:
		virtual bool Modify(Chat &chat);
		virtual std::string GetDescription() const;
	};
}