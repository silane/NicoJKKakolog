#pragma once
#include "OnceASecondChatProvider.h"
#include "Chat.h"

namespace NicoJKKakolog
{
	class TestChatProvider :
		public OnceASecondChatProvider
	{
	private:
		std::chrono::system_clock::time_point t;
	public:
		TestChatProvider();
		virtual std::vector<Chat> GetOnceASecond(const ChannelInfo &channel, std::chrono::system_clock::time_point t) override;
	};
}