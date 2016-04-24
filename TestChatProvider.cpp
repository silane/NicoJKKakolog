#include "stdafx.h"
#include "TestChatProvider.h"
#include <ctime>

namespace NicoJKKakolog
{
	TestChatProvider::TestChatProvider() :
		OnceASecondChatProvider(std::chrono::seconds(10))
	{
	}

	std::vector<Chat> TestChatProvider::GetOnceASecond(const ChannelInfo &channel, std::chrono::system_clock::time_point t)
	{
		time_t tt = std::chrono::system_clock::to_time_t(t);
		return { Chat(0, "sid : " + std::to_string(channel.ServiceId) + " t : " + ctime(&tt), "", 0, NicoJKKakolog::Chat::Position::Up) };
	}
}