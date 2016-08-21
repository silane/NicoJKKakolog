#include "../../stdafx.h"
#include "WordNgChatModRule.h"

namespace NicoJKKakolog
{
	WordNgChatModRule::WordNgChatModRule(const std::string &ngword):
		ngword(ngword)
	{
	}

	bool WordNgChatModRule::Modify(Chat &chat)
	{
		if (chat.text.find(ngword) == std::string::npos)
			return false;

		chat.ng = true;
		return true;
	}

	std::string WordNgChatModRule::GetDescription()
	{
		return u8"NGWord: " + this->ngword;
	}

	std::string WordNgChatModRule::GetWord() const
	{
		return this->ngword;
	}
}