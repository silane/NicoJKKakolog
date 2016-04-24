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
		//�w�莞�Ԉȏ�Ԋu�������Ə������Ȃ�(�V�[�N�����H)
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
