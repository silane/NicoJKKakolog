#pragma once
#include "ChatModRule.h"
#include <string>

namespace NicoJKKakolog
{
	class IdNgChatModRule :public ChatModRule
	{
	private:
		std::string ngid;

	public:
		explicit IdNgChatModRule(const std::string &ngid);
		virtual ~IdNgChatModRule();

		virtual bool Modify(Chat &chat) override;
		virtual std::string GetDescription() override;

		std::string GetId() const;
	};
}