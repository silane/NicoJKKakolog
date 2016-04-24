#include "stdafx.h"
#include "NiconicoJikkyouLogChatProvider.h"
#include "NiconicoLoginSession.h"

namespace NicoJKKakolog {
	NiconicoJikkyouLogChatProvider::NiconicoJikkyouLogChatProvider(const std::unordered_map<uint_least32_t, int> &jkidTable,const NiconicoLoginSession *login):
		OnceASecondChatProvider(std::chrono::seconds(10)), client(U("http://jk.nicovideo.jp")), jkidTable(jkidTable), chatCollectTask([] {}),login(login)
	{
	}

	NiconicoJikkyouLogChatProvider::~NiconicoJikkyouLogChatProvider()
	{
		this->chatCollectTask.wait();
	}

	std::vector<Chat> NiconicoJikkyouLogChatProvider::GetOnceASecond(const ChannelInfo & channel, std::chrono::system_clock::time_point t)
	{
		std::vector<Chat> ret;

		int jkID;
		try {
			uint_least16_t nid = channel.NetworkId;
			jkID = jkidTable.at(((uint_least32_t)(channel.ServiceId) << 16) | ((nid == 0) ? 0xF : nid));
		}
		catch (std::out_of_range) {
			return{};
		}

		{
			std::lock_guard<std::mutex> lock(this->chatsMutex);
			auto range = chats.equal_range(std::chrono::system_clock::to_time_t(t));
			std::for_each(range.first, range.second, [&ret](const std::unordered_multimap<std::time_t, Chat>::value_type &x) {ret.push_back(x.second); });
		}

		if (t - lastGetTime < std::chrono::seconds(10) && lastGetTime - t < std::chrono::seconds(10))
			return ret;

		//10•b‚Éˆê‰ñŽæ“¾‚·‚é
		lastGetTime = t;

		std::time_t startTime = std::chrono::system_clock::to_time_t(t);
		std::time_t endTime = std::chrono::system_clock::to_time_t(std::chrono::seconds(10) + t);

		web::http::http_request req;
		req.headers().add(U("Cookie"), login->GetUserSessionCookie().c_str());
		req.set_request_uri(U("/api/getflv?v=jk") + utility::conversions::to_string_t(std::to_string(jkID)) +
			U("&start_time=") + utility::conversions::to_string_t(std::to_string(startTime)) +
			U("&end_time=") + utility::conversions::to_string_t(std::to_string(endTime)));

		this->chatCollectTask.wait();
		this->chatCollectTask=client.request(req).then([this, endTime](web::http::http_response &response) {
			//”ñ“¯ŠúŽÀs•”•ª
			auto query = web::uri::split_query(response.extract_string().get());
			if (query.count(U("error")))
				return;
			utility::string_t thread_id = query[U("thread_id")];
			utility::string_t ms = query[U("ms")];
			utility::string_t http_port = query[U("http_port")];
			utility::string_t user_id = query[U("user_id")];
			//utility::string_t end_time = query[U("end_time")];

			web::http::http_request req;
			req.headers().add(U("Cookie"), this->login->GetUserSessionCookie().c_str());
			req.set_request_uri(U("/api/v2/getwaybackkey?thread=") + thread_id);
			web::http::http_response res = this->client.request(req).get();
			utility::string_t waybackKey = res.extract_string().get();
			waybackKey = waybackKey.substr(waybackKey.find('=') + 1);

			web::http::client::http_client cli(U("http://") + ms + U(":") + http_port +
				U("/api/thread?thread=") + thread_id +
				U("&res_from=-100&version=20061206&when=") + utility::conversions::print_string(endTime) +
				U("&user_id=") + user_id +
				U("&waybackkey=") + waybackKey +
				U("&scores=1"));

			req = web::http::http_request();
			req.headers().add(U("Cookie"), this->login->GetUserSessionCookie().c_str());
			std::vector<unsigned char> data=cli.request(req).get().extract_vector().get();
			this->parser.PushString(std::string((char*) data.data(),data.size()));
			while (this->parser.HasChat())
			{
				std::lock_guard<std::mutex> lock(this->chatsMutex);
				auto pair = this->parser.PopChat();
				if (this->chats.size() == 0 || this->chats.rbegin()->second.number < pair.second.number)
				{
					this->chats.insert({ pair.first,  pair.second });
				}
			}
			this->parser.Reset();
		});

		return ret;
	}
}