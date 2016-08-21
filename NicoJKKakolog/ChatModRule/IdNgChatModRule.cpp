#include "../../stdafx.h"
#include "IdNgChatModRule.h"

namespace NicoJKKakolog
{
	IdNgChatModRule::IdNgChatModRule(const std::string &ngid):
		ngid(ngid)
	{
	}


	IdNgChatModRule::~IdNgChatModRule()
	{
	}

	bool IdNgChatModRule::Modify(Chat &chat)
	{
		if (chat.userId != this->ngid)
			return false;

		chat.ng = true;
		return true;
	}

	std::string IdNgChatModRule::GetDescription()
	{
		return u8"NGID: " + ngid;
	}

	std::string IdNgChatModRule::GetId() const
	{
		return this->ngid;
	}
}