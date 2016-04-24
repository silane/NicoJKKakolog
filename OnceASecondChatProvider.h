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
		//continuousCallLimit�ȏ㎞�Ԃ��󂭂ƃV�[�N�����Ɣ��肵GetOnceASecond��A���I�ɌĂ΂Ȃ�
		OnceASecondChatProvider(const std::chrono::seconds &continuousCallLimit);
		virtual std::vector<Chat> Get(const ChannelInfo &channel, std::chrono::system_clock::time_point t) override;
		virtual std::vector<Chat> GetOnceASecond(const ChannelInfo &channel, std::chrono::system_clock::time_point t)=0;
		virtual ~OnceASecondChatProvider();
	};
}

