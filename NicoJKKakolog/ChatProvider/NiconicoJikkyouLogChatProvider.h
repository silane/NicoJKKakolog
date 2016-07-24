#pragma once
#include "OnceASecondChatProvider.h"
#include "../NiconicoUtils/NiconicoLoginSession.h"
#include "../NiconicoUtils/NiconicoJikkyouXmlParser.h"
#include <map>
#include <unordered_map>
#include <cpprest\http_client.h>

namespace NicoJKKakolog {

	//再生ファイルに対応するニコニコ実況過去ログをとってくるプロバイダー
	class NiconicoJikkyouLogChatProvider :
		public OnceASecondChatProvider
	{
	private:
		web::http::client::http_client client;
		std::unordered_map<uint_least32_t, int> jkidTable;
		NiconicoLoginSession *login;
		NiconicoJikkyouXmlParser parser;

		std::mutex chatsMutex;
		std::multimap<std::time_t, Chat> chats;

		pplx::task<void> chatCollectTask;

		int lastJkId;
		std::chrono::system_clock::time_point lastGetTime;

	public:
		NiconicoJikkyouLogChatProvider(const std::unordered_map<uint_least32_t, int> &jkidTable,NiconicoLoginSession *login);
		virtual ~NiconicoJikkyouLogChatProvider() noexcept override;

		virtual std::vector<Chat> GetOnceASecond(const ChannelInfo & channel, std::chrono::system_clock::time_point t) override;
	};
}
