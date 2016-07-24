#pragma once

#include "OnceASecondChatProvider.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>
#include "../../NichanParser/NichanParser.h"
#include "../NichanUtils/INichanThreadSelector.h"

namespace NicoJKKakolog
{
	class NichanChatProvider :public OnceASecondChatProvider
	{
	private:
		std::vector<Nichan::Res> chats;
		std::thread thCollectRes;
		//std::thread thSearchThre;
		std::mutex mtxThreads;
		std::mutex mtxChats;
		std::atomic<bool> stop;

		std::chrono::system_clock::duration resCollectInterval;
		std::chrono::system_clock::duration threSearchInterval;
		Color chatColor;

		INichanThreadSelector *threadSelector;

	private:
		static void CollectRes(NichanChatProvider *this_);
	public:
		NichanChatProvider(const Color &chatColor,const std::chrono::milliseconds &resCollectInterval, const std::chrono::milliseconds &threSearchInterval, INichanThreadSelector *threadSelector);
		virtual ~NichanChatProvider() noexcept override;

		virtual std::vector<Chat> GetOnceASecond(const ChannelInfo &channel, std::chrono::system_clock::time_point t) override;
	};
}