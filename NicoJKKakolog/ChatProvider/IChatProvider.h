#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <exception>

#include "../Chat.h"
#include "../ChannelInfo.h"

namespace NicoJKKakolog
{
	//ChatProvider::Get�œ�����ƃ��[�U�[�Ƀ��b�Z�[�W��\�����Ă���ChatProvider�̗��p�𒆎~����
	class ChatProviderError:public std::exception
	{
	private:
		std::string what_arg;
	public:
		explicit ChatProviderError(const std::string &what_arg):
			what_arg(what_arg)
		{}

		virtual const char *what() const override
		{
			return what_arg.c_str();
		}
	};

	//�^����ꂽ�`�����l���Ǝ����̃`���b�g��񋟂���
	class IChatProvider
	{
	public:
		//channel�Ǝ���t�̏�񂩂�Ή�����`���b�g��Ԃ��悤�Ɏ�������BChannelInfo.h���Q��
		virtual std::vector<Chat> Get(const ChannelInfo &channel, std::chrono::system_clock::time_point t) = 0;
		virtual ~IChatProvider()=default;
	};
}