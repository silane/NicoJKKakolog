#pragma once
#include "IChatModRule.h"
#include <string>

namespace NicoJKKakolog
{
	class IdNgChatModRule :public IChatModRule
	{
	private:
		std::string ngid;

	public:
		explicit IdNgChatModRule(const std::string &ngid);
		
		virtual bool Modify(Chat &chat) override;
		virtual std::string GetDescription() const override;

		std::string GetId() const;
	};
}