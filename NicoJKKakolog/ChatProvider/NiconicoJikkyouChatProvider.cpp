#include "../../stdafx.h"
#include "NiconicoJikkyouChatProvider.h"
//#include "NetworkServiceIDTable.h"
//#include <sstream>
#include <pplx\pplxtasks.h>
#include "../../PracticalSocket/PracticalSocket.h"

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
			jkID = jkidTable.at(((uint_least32_t)(channel.ServiceId) << 16) | ((nid == 0) ? 0xF : nid));
		}catch (std::out_of_range) {
			return{};
		}
		if (lastJKId == jkID)
		{
			//�񓯊������ŏW�߂��f�[�^����`���b�g�𐶐����ĕԂ�
			std::lock_guard<std::mutex> lock(this->parserMutex);
			std::vector<Chat> ret;

			while (parser.HasChat())
			{
				ret.push_back(parser.PopChat().second);
			}
			return ret;
		}

		//�I��ԑg�ɕύX���������ꍇ
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

				//�񓯊����s����
				auto query = web::uri::split_query(response.extract_string().get());
				if (query.count(U("error")))
					return;
				utility::string_t thread_id = query[U("thread_id")];
				utility::string_t ms = query[U("ms")];
				utility::string_t ms_port = query[U("ms_port")];

				std::string body("<thread res_from=\"-10\" version=\"20061206\" thread=\"" + utility::conversions::to_utf8string(thread_id) + "\" />");
				body.push_back('\0');
				TCPSocket socket(utility::conversions::to_utf8string( ms),(unsigned short)std::stoi( utility::conversions::to_utf8string( ms_port)));
				socket.send(body.c_str(), body.size());

				if (ct.is_canceled())
					return;

				//�R�����g��M���[�v
				while (true) {
					constexpr int BUFFER_SIZE = 2048;
					char buf[BUFFER_SIZE];
					int ret=socket.recv(buf, BUFFER_SIZE);
					if (ct.is_canceled())
						break;

					if (ret < 0)
						throw SocketException("Error in recv function");
					if (ret == EOF)
						break;
					{
						std::lock_guard<std::mutex> lock(this->parserMutex);
						this->parser.PushString(std::string(buf, ret));
					}
				}
			});
		}
		catch (const web::http::http_exception &e) {
			//const char *m = e.what();
		}
		catch (const SocketException &e) {
			//const char *m = e.what();
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