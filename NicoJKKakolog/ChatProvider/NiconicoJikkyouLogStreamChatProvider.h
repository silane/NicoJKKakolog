#pragma once
#include "OnceASecondChatProvider.h"
#include "../NiconicoUtils/NiconicoJikkyouXmlParser.h"
#include <pplx\pplxtasks.h>
#include <unordered_map>
#include <mutex>
#include <istream>

namespace NicoJKKakolog
{
	//ストリームからニコニコ実況コメントをとってくるプロバイダー
	//開始時刻からの相対時刻でなく絶対時刻でコメントを返す（再放送で困る）
	//つまり未完成
	class NiconicoJikkyouLogStreamChatProvider :
		public OnceASecondChatProvider
	{
	private:
		std::mutex chatsMutex;
		std::unordered_multimap<std::time_t,Chat> chats;
		NiconicoJikkyouXmlParser parser;
		pplx::task<void> fileReadTask;

	public:
		NiconicoJikkyouLogStreamChatProvider();
		NiconicoJikkyouLogStreamChatProvider(std::istream &input);
		virtual ~NiconicoJikkyouLogStreamChatProvider() override;

		virtual void SetStream(std::istream &input);

		virtual std::vector<Chat> GetOnceASecond(const ChannelInfo & channel, std::chrono::system_clock::time_point t) override;
	};

}