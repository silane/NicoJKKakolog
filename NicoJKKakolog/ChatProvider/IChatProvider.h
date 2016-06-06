#pragma once

#include <string>
#include <vector>
#include <chrono>

#include "../Chat.h"
#include "../ChannelInfo.h"

namespace NicoJKKakolog
{
	//�^����ꂽ�`�����l���Ǝ����̃`���b�g��񋟂���
	class IChatProvider
	{
	public:
		//channel�Ǝ���t�̏�񂩂�Ή�����`���b�g��Ԃ��悤�Ɏ�������BChannelInfo.h���Q��
		virtual std::vector<Chat> Get(const ChannelInfo &channel, std::chrono::system_clock::time_point t) = 0;
		virtual ~IChatProvider()=default;
	};
}