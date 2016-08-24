#include "../../stdafx.h"
#include "NiconicoJikkyouChatProvider.h"
#include <pplx/pplxtasks.h>
#include "../../MySock/Socket.h"
#include "../../MySock/EndPoint/IPEndPoint.h"
#include "../../MySock/Dns.h"

namespace NicoJKKakolog
{
	NiconicoJikkyouChatProvider::NiconicoJikkyouChatProvider(const std::unordered_map<uint_least32_t, int> &jkidTable):
		client(U("http://jk.nicovideo.jp")),jkidTable(jkidTable),lastJKId(0),parser(true)
	{
	}

	std::vector<Chat> NiconicoJikkyouChatProvider::Get(const ChannelInfo &channel, std::chrono::system_clock::time_point t)
	{
		int jkID;
		try	{
			uint_least16_t nid = channel.NetworkId;
			//nidが地上波の範囲ならnid=0xfにして取得
			jkID = jkidTable.at(((uint_least32_t)(channel.ServiceId) << 16) | ((0x7880 <= nid && nid <= 0x7fe8) ? 0xf : nid));
		}catch (std::out_of_range) {
			return{};
		}
		if (lastJKId == jkID)
		{
			//非同期部分で集めたデータからチャットを生成して返す
			std::lock_guard<std::mutex> lock(this->parserMutex);
			std::vector<Chat> ret;

			while (parser.HasChat())
			{
				ret.push_back(parser.PopChat().second);
			}
			return ret;
		}

		//選択番組に変更があった場合
		lastJKId = jkID;
		try
		{
			this->cancelSource.cancel();
			//this->chatCollectTask.wait();
			this->cancelSource = pplx::cancellation_token_source();
			pplx::cancellation_token ct=this->cancelSource.get_token();
			{
				std::lock_guard<std::mutex> lock(this->parserMutex);
				this->parser.Reset();
			}

			this->chatCollectTask=client.request(web::http::methods::GET, U("/api/getflv?v=jk") + std::to_wstring(jkID),ct)
				.then([this,ct](web::http::http_response response) {

				//非同期実行部分
				auto query = web::uri::split_query(response.extract_string().get());
				if (query.count(U("error")))
					return;
				utility::string_t thread_id = query[U("thread_id")];
				utility::string_t ms = query[U("ms")];
				utility::string_t ms_port = query[U("ms_port")];

				std::string body("<thread res_from=\"-10\" version=\"20061206\" thread=\"" + utility::conversions::to_utf8string(thread_id) + "\" />");
				body.push_back('\0');
				MySock::Socket socket(AF_INET, SOCK_STREAM, 0);
				socket.SetReceiveTimeout(3000);
				auto addresses = MySock::Dns::GetHostAddresses(utility::conversions::to_utf8string(ms));
				if (addresses.empty())
					throw std::runtime_error("couldn't resolve host name");
				socket.Connect(MySock::IPEndPoint(addresses[0], (unsigned short)std::stoi(utility::conversions::to_utf8string(ms_port))));
				socket.Send(body.c_str(), (int)body.size());

				if (ct.is_canceled())
					return;

				//コメント受信ループ
				while (true) {
					constexpr int BUFFER_SIZE = 2048;
					char buf[BUFFER_SIZE];
					int ret;
					try
					{
						ret = socket.Receive(buf, BUFFER_SIZE);
					}
					catch (const MySock::TimeoutError &)
					{
					}

					if (ct.is_canceled())
						break;

					if (ret == 0)
						break;
					
					{
						std::lock_guard<std::mutex> lock(this->parserMutex);
						this->parser.PushString(std::string(buf, ret));
					}
				}
			});
		}
		catch (const std::exception &e) {
			throw ChatProviderError(std::string("ニコニコ実況でエラーが発生しました: ")+e.what());
		}

		return{};
	}

	NiconicoJikkyouChatProvider::~NiconicoJikkyouChatProvider() noexcept
	{
		try {
			this->cancelSource.cancel();
			this->chatCollectTask.wait();
		}
		catch (...) {}
	}
}