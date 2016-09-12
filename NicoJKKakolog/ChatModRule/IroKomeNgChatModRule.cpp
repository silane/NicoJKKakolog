#include "../../stdafx.h"
#include "IroKomeNgChatModRule.h"

namespace NicoJKKakolog
{
	bool IroKomeNgChatModRule::Modify(Chat &chat)
	{
		if (chat.color == RGB(255, 255, 255))
			return false;

		chat.ng = true;
		return true;
	}
	std::string IroKomeNgChatModRule::GetDescription() const
	{
		return u8"êFÉRÉÅNG";
	}
}