#pragma once
#include "IChatProvider.h"
#include "../NiconicoUtils/NiconicoJikkyouXmlParser.h"
#include <unordered_map>
#include <cpprest\http_client.h>

namespace NicoJKKakolog
{
	//現在のニコニコ実況コメントをとってくるプロバイダー
	class NiconicoJikkyouChatProvider :
		public IChatProvider
	{
	private:
		std::unordered_map<uint_least32_t, int> jkidTable;
		web::http::client::http_client client;

		NiconicoJikkyouXmlParser parser;

		int lastJKId;

		std::mutex parserMutex;
		
		concurrency::task<void> chatCollectTask;
		concurrency::cancellation_token_source cancelSource;

	public:
		NiconicoJikkyouChatProvider(const std::unordered_map<uint_least32_t, int> &jkidTable);
		virtual std::vector<Chat> Get(const ChannelInfo &channel, std::chrono::system_clock::time_point t) override;
		virtual ~NiconicoJikkyouChatProvider() override;
	};
}