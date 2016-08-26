#pragma once
#include "IChatModRule.h"

namespace NicoJKKakolog
{
	class WordNgChatModRule :public IChatModRule
	{
	private:
		std::string ngword;
	public:
		WordNgChatModRule(const std::string &ngword);
		virtual ~WordNgChatModRule()=default;

		virtual bool Modify(Chat &chat) override;
		virtual std::string GetDescription() const override;

		std::string GetWord() const;
	};
}