#pragma once
#include "IChatProvider.h"
#include <chrono>

namespace NicoJKKakolog
{
	class OnceASecondChatProvider :
		public IChatProvider
	{
	private:
		std::chrono::system_clock::time_point lasttime;
		std::chrono::seconds continuousCallLimit;

	public:
		//continuousCallLimit以上時間が空くとシークしたと判定しGetOnceASecondを連続的に呼ばない
		OnceASecondChatProvider(const std::chrono::seconds &continuousCallLimit);
		virtual std::vector<Chat> Get(const ChannelInfo &channel, std::chrono::system_clock::time_point t) override;
		virtual std::vector<Chat> GetOnceASecond(const ChannelInfo &channel, std::chrono::system_clock::time_point t)=0;
		virtual ~OnceASecondChatProvider();
	};
}

