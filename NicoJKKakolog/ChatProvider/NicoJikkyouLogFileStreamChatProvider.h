#pragma once
#include "NiconicoJikkyouLogStreamChatProvider.h"
#include <fstream>

namespace NicoJKKakolog
{
	class NicoJikkyouLogFileStreamChatProvider :
		public NiconicoJikkyouLogStreamChatProvider
	{
	private:
		std::ifstream ifs;

	public:
		NicoJikkyouLogFileStreamChatProvider(const std::string &path);
		virtual ~NicoJikkyouLogFileStreamChatProvider() noexcept override=default;
	};

}