#include "../../stdafx.h"
#include "JyougeKomeNgChatModRule.h"

namespace NicoJKKakolog
{
	bool JyougeKomeNgChatModRule::Modify(Chat &chat)
	{
		if (chat.position == Chat::Position::Default)
			return false;
		chat.ng = true;
		return true;
	}

	std::string JyougeKomeNgChatModRule::GetDescription() const
	{
		return u8"è„â∫ÉRÉÅNG";
	}
}