#pragma once
#include <string>
#include <vector>
#include <chrono>
#include "../ChannelInfo.h"

namespace NicoJKKakolog
{
	class INichanThreadSelector
	{
	public:

		virtual std::vector<std::string> Get(const ChannelInfo &channel, const std::chrono::system_clock::time_point &t) = 0;

		virtual ~INichanThreadSelector() = default;
	};
}
