#pragma once

#include "../Chat.h"

namespace NicoJKKakolog
{
	//チャットの修正ルール(NG化を含む)
	class IChatModRule
	{
	public:
		virtual ~IChatModRule()=default;

		//戻り値は変更を行ったかどうか
		virtual bool Modify(Chat &chat)=0;

		//ユーザーに表示する説明
		virtual std::string GetDescription() const = 0;
	};
}