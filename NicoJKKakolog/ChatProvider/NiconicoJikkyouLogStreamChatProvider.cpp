#include "../../stdafx.h"
#include "NiconicoJikkyouLogStreamChatProvider.h"
#include <utility>

namespace NicoJKKakolog
{
	NiconicoJikkyouLogStreamChatProvider::NiconicoJikkyouLogStreamChatProvider():
		OnceASecondChatProvider(std::chrono::seconds(10))
	{
	}

	NiconicoJikkyouLogStreamChatProvider::NiconicoJikkyouLogStreamChatProvider(std::istream &input):
		OnceASecondChatProvider(std::chrono::seconds(10)), fileReadTask([] {})
	{
		SetStream(input);
	}

	NiconicoJikkyouLogStreamChatProvider::~NiconicoJikkyouLogStreamChatProvider() noexcept
	{
		try {
			this->fileReadTask.wait();
		}
		catch (...) {}
	}

	void NiconicoJikkyouLogStreamChatProvider::SetStream(std::istream &input)
	{
		this->fileReadTask=pplx::task<void>([this,&input]() {
			while (!input.fail() && !input.eof())
			{
				std::string buf;
				std::getline(input, buf);
				parser.PushString(buf);

				if (parser.HasChat())
				{
					std::lock_guard<std::mutex> lock(chatsMutex);
					auto pair = parser.PopChat();
					chats.insert({ pair.first,pair.second });
				}
			}
		});
	}

	std::vector<Chat> NiconicoJikkyouLogStreamChatProvider::GetOnceASecond(const ChannelInfo & channel, std::chrono::system_clock::time_point t)
	{
		std::vector<Chat> ret;
		decltype(chats.equal_range(std::chrono::system_clock::to_time_t(t))) range;
		{
			std::lock_guard<std::mutex> lock(chatsMutex);
			range=chats.equal_range(std::chrono::system_clock::to_time_t(t));
		}
		
		std::for_each(range.first, range.second, [&ret](const std::unordered_multimap<std::time_t, Chat>::value_type &x) {ret.push_back(x.second); });
		return ret;
	}
}