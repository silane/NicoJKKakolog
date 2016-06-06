#pragma once
#include "OnceASecondChatProvider.h"
#include "../NiconicoUtils/NiconicoJikkyouXmlParser.h"
#include <pplx\pplxtasks.h>
#include <unordered_map>
#include <mutex>
#include <istream>

namespace NicoJKKakolog
{
	//�X�g���[������j�R�j�R�����R�����g���Ƃ��Ă���v���o�C�_�[
	//�J�n��������̑��Ύ����łȂ���Ύ����ŃR�����g��Ԃ��i�ĕ����ō���j
	//�܂薢����
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