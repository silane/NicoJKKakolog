#include "../../stdafx.h"
#include "NichanChatProvider.h"
#include <codecvt>

namespace NicoJKKakolog
{
	NichanChatProvider::NichanChatProvider(const Color &chatColor,const std::chrono::milliseconds &resCollectInterval, const std::chrono::milliseconds &threSearchInterval, INichanThreadSelector *threadSelector):
		stop(false),resCollectInterval(resCollectInterval),threSearchInterval(threSearchInterval),threadSelector(threadSelector),chatColor(chatColor)
	{
		thCollectRes = std::thread(CollectRes, this);
	}

	NichanChatProvider::~NichanChatProvider() noexcept
	{
		try
		{
			this->stop = true;
			this->thCollectRes.join();
			delete this->threadSelector;
		}catch(...){}
	}

	void NichanChatProvider::CollectRes(NichanChatProvider *this_)
	{
		try
		{
			std::unordered_map<std::string, int> threads;//何レス目まで表示したか

			auto count = this_->threSearchInterval / this_->resCollectInterval;
			if (count <= 0)
				count = 1;

			for (int i = 0; !this_->stop; i++)
			{
				ChannelInfo ci;//ダミー
				std::chrono::system_clock::time_point tp;//ダミー

				if (i%count == 0)
				{
					i = 0;
					for (const std::string &url : this_->threadSelector->Get(ci, tp))
					{
						if (threads.count(url) == 0)
							threads[url] = 0;
					}
				}

				for (auto &&pair : threads)
				{
					Nichan::Thread th = Nichan::ParseThreadFromUrl(pair.first);

					for (auto itr = th.res.rbegin(); itr != th.res.rend(); ++itr)
					{
						if (itr->number <= pair.second)
							break;

						{
							std::lock_guard<std::mutex> lock(this_->mtxChats);

							//static std::wstring_convert<std::codecvt_utf8<wchar_t>,wchar_t> cvt;
							//OutputDebugStringW((cvt.from_bytes(pair.first)+L": "+ cvt.from_bytes(itr->message)+L'\n').c_str());

							this_->chats.push_back(std::move(*itr));
						}
					}
					pair.second = th.res.back().number;
				}
				std::this_thread::sleep_for(this_->resCollectInterval);
			}
		}
		catch (...)
		{
			this_->stop = true;
		}
	}

	std::vector<Chat> NichanChatProvider::GetOnceASecond(const ChannelInfo &channel, std::chrono::system_clock::time_point t)
	{
		std::vector<Chat> ret;

		std::lock_guard<std::mutex> lock(this->mtxChats);
		for (const auto &res : this->chats)
		{
			if (t - res.date > std::chrono::seconds(120))
					continue;
			Chat chat;
			chat.text = res.message;
			chat.userId = res.id;
			if (this->chatColor != ColorNone)
				chat.color = this->chatColor;
			else
				chat.color = ColorHLSToRGB(std::rand() % 360, (WORD)(255*0.7), 255);
			ret.push_back(std::move(chat));
		}
		this->chats.clear();

		return ret;
	}
}