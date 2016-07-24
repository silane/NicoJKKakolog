#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <exception>

#include "../Chat.h"
#include "../ChannelInfo.h"

namespace NicoJKKakolog
{
	//ChatProvider::Getで投げるとユーザーにメッセージを表示してそのChatProviderの利用を中止する
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

	//与えられたチャンネルと時刻のチャットを提供する
	class IChatProvider
	{
	public:
		//channelと時刻tの情報から対応するチャットを返すように実装する。ChannelInfo.hも参照
		virtual std::vector<Chat> Get(const ChannelInfo &channel, std::chrono::system_clock::time_point t) = 0;
		virtual ~IChatProvider()=default;
	};
}