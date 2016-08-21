#pragma once
#include "ChatModRule.h"

namespace NicoJKKakolog
{
	class WordNgChatModRule :public ChatModRule
	{
	private:
		std::string ngword;
	public:
		WordNgChatModRule(const std::string &ngword);
		virtual ~WordNgChatModRule()=default;

		virtual bool Modify(Chat &chat) override;
		virtual std::string GetDescription() override;
	};
}