#include "stdafx.h"
#include "OnceASecondChatProvider.h"

namespace NicoJKKakolog 
{
	OnceASecondChatProvider::OnceASecondChatProvider(const std::chrono::seconds &continuousCallLimit):
		continuousCallLimit(continuousCallLimit)
	{
	}

	std::vector<Chat> OnceASecondChatProvider::Get(const ChannelInfo &channel, std::chrono::system_clock::time_point t)
	{
		std::vector<Chat> ret;
		//指定時間以上間隔があくと処理しない(シークした？)
		if (lasttime - t < continuousCallLimit && t - lasttime < continuousCallLimit)
		{
			for (; lasttime <= t; lasttime += std::chrono::seconds(1))
			{
				auto chats = GetOnceASecond(channel, t);
				ret.insert(std::end(ret), std::begin(chats), std::end(chats));
			}
		}else {
			lasttime = t;
		}

		return ret;
	}

	OnceASecondChatProvider::~OnceASecondChatProvider()
	{
	}
}
