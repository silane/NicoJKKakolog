#pragma once

#include <string>
#include <vector>
#include <chrono>

#include "../Chat.h"
#include "../ChannelInfo.h"

namespace NicoJKKakolog
{
	//与えられたチャンネルと時刻のチャットを提供する
	class IChatProvider
	{
	public:
		//channelと時刻tの情報から対応するチャットを返すように実装する。ChannelInfo.hも参照
		virtual std::vector<Chat> Get(const ChannelInfo &channel, std::chrono::system_clock::time_point t) = 0;
		virtual ~IChatProvider()=default;
	};
}