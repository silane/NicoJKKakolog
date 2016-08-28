#include "../../stdafx.h"
#include "JyougeIroKomeNgChatModRule.h"

namespace NicoJKKakolog
{
	bool JyougeIroKomeNgChatModRule::Modify(Chat &chat)
	{
		if (chat.position == Chat::Position::Default || chat.color == RGB(255, 255, 255))
			return false;

		chat.ng = true;
		return true;
	}

	std::string JyougeIroKomeNgChatModRule::GetDescription() const
	{
		return u8"è„â∫ÉRÉÅÇ©Ç¬êFÉRÉÅNG";
	}
}