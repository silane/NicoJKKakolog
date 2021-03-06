#include "../../stdafx.h"
#include "NiconicoJikkyouLogChatProvider.h"
#include "../NiconicoUtils/NiconicoLoginSession.h"

namespace NicoJKKakolog {
	NiconicoJikkyouLogChatProvider::NiconicoJikkyouLogChatProvider(const std::unordered_map<uint_least32_t, int> &jkidTable,NiconicoLoginSession *login):
		OnceASecondChatProvider(std::chrono::seconds(10)), 
		jkidTable(jkidTable), chatCollectTask([] {}),login(login),lastJkId(0)
	{
	}

	NiconicoJikkyouLogChatProvider::~NiconicoJikkyouLogChatProvider() noexcept
	{
		try
		{
			this->chatCollectTask.wait();
		}catch(...){}
	}

	std::vector<Chat> NiconicoJikkyouLogChatProvider::GetOnceASecond(const ChannelInfo & channel, std::chrono::system_clock::time_point t)
	{
		std::vector<Chat> ret;

		int jkID;
		{
			decltype(this->jkidTable)::const_iterator itr;
			uint_least16_t nid = channel.NetworkId;

			if (nid != 0)//録画ファイルのNIDは取得できてない(0になってる)可能性がある
				itr = this->jkidTable.find(((uint_least32_t)channel.ServiceId << 16) | nid);
			else
			{
				itr = this->jkidTable.find(((uint_least32_t)(channel.ServiceId << 16)) | 0xF);//地上波にないか検索
				if (itr == std::end(this->jkidTable))
					itr = this->jkidTable.find(((uint_least32_t)channel.ServiceId << 16) | 0x4);//BSにないか検索
			}
			if (itr == std::end(this->jkidTable))
				return{};
			jkID = itr->second;
		}

		{
			std::lock_guard<std::mutex> lock(this->chatsMutex);
			if (lastJkId == jkID)
			{
				auto range = this->chats.equal_range(std::chrono::system_clock::to_time_t(t));
				std::for_each(range.first, range.second, [&ret](const decltype(this->chats)::value_type &x) {ret.push_back(x.second); });
				this->chats.erase(range.first, range.second);
			}
		}

		if ((t - lastGetTime < std::chrono::seconds(10) && lastGetTime - t < std::chrono::seconds(10) && lastJkId == jkID) ||
			!this->chatCollectTask.is_done())
			return ret;

		//10秒に一回かチャンネルが変わったら取得する

		lastJkId = jkID;
		lastGetTime = t;

		try
		{
			this->chatCollectTask.wait();//this->loginへ同時アクセス防止のためこの位置
		}
		catch (std::exception)
		{
			throw ChatProviderError("ニコニコ実況過去ログのコメントの取得に失敗しました。このエラーはサーバー混雑時にも起こり得ます。");
		}

		//シークとかしてなければ前回取得分以降のコメ取得、そうでなければ250個分取得
		int resFrom;
		if (!this->chats.empty() && t - lastGetTime < std::chrono::seconds(30) && lastGetTime - t < std::chrono::seconds(0) && lastJkId == jkID)
		{
			resFrom = this->chats.rbegin()->second.number;
		}
		else
		{
			resFrom = -250;
			this->chats.clear();
		}
		
		this->chatCollectTask = this->CollectChat(jkID, t, t + std::chrono::seconds(10),resFrom)
			.then([this](const std::vector<std::pair<std::time_t, Chat>> &newChats) {
			std::lock_guard<std::mutex> lock(this->chatsMutex);
			for (const auto &chat : newChats)
				this->chats.emplace_hint(this->chats.end(), chat.first, chat.second);
		});

		return ret;
	}
	pplx::task<std::vector<std::pair<std::time_t, Chat>>> NiconicoJikkyouLogChatProvider::CollectChat(int jkId, const std::chrono::system_clock::time_point &startTime, const std::chrono::system_clock::time_point &endTime, int resFrom)
	{
		web::http::client::http_client_config clientConfig;
		clientConfig.set_timeout(std::chrono::seconds(10));
		web::http::client::http_client client(U("http://jk.nicovideo.jp"),clientConfig);
		web::http::http_request req;
		std::string cookie = this->login->GetUserSessionCookie();
		utility::string_t endTimeStr = utility::conversions::print_string(std::chrono::system_clock::to_time_t(endTime));

		req.headers().add(U("Cookie"), cookie.c_str());
		req.set_request_uri(U("/api/getflv?v=jk") + utility::conversions::print_string(jkId) +
			U("&start_time=") + utility::conversions::print_string(std::chrono::system_clock::to_time_t(startTime)) +
			U("&end_time=") + endTimeStr);

		return client.request(req).then([endTimeStr, resFrom, cookie = std::move(cookie),clientConfig=std::move(clientConfig)](web::http::http_response &response) {
			//非同期実行部分
			auto query = web::uri::split_query(response.extract_string().get());
			if (query.count(U("error")))
			{
				throw std::runtime_error("failed to download");
				//this->login->Relogin();
				//return;
			}
			utility::string_t thread_id = query[U("thread_id")];
			utility::string_t ms = query[U("ms")];
			utility::string_t http_port = query[U("http_port")];
			utility::string_t user_id = query[U("user_id")];
			
			
			web::http::http_request req;
			req.headers().add(U("Cookie"), cookie.c_str());

			web::http::client::http_client client(U("http://jk.nicovideo.jp"), clientConfig);
			req.set_request_uri(U("/api/v2/getwaybackkey?thread=") + thread_id);
			web::http::http_response res = client.request(req).get();
			utility::string_t waybackKey = res.extract_string().get();
			waybackKey = waybackKey.substr(waybackKey.find('=') + 1);

			web::http::client::http_client cli(U("http://") + ms + U(":") + http_port, clientConfig);
			req.set_request_uri(U("/api/thread?thread=") + thread_id +
				U("&res_from=")+utility::conversions::print_string(resFrom)+
				U("&version=20061206&when=") + endTimeStr +
				U("&user_id=") + user_id +
				U("&waybackkey=") + waybackKey +
				U("&scores=1"));
			std::vector<unsigned char> data = cli.request(req).get().extract_vector().get();

			NiconicoJikkyouXmlParser parser;
			parser.PushString(std::string((char*)data.data(), data.size()));
			std::vector<std::pair<std::time_t, Chat>> ret;
			while (parser.HasChat())
			{
				auto pair = parser.PopChat();
				ret.emplace_back(pair.first,  pair.second);
			}
			return ret;
		});
	}

}

