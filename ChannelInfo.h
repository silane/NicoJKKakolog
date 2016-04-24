#pragma once
#include <cstdint>
#include <string>
#include <chrono>

namespace NicoJKKakolog
{
	//ChatProviderのGetに渡されるチャンネル情報
	//すべてが埋まっているとは限らない。特に録画再生時。
	struct ChannelInfo
	{
		uint_least16_t NetworkId=0;
		uint_least16_t TransportstreamId=0;
		uint_least16_t ServiceId=0;
		uint_least16_t EventId=0;

		std::wstring NetworkName;
		std::wstring TransportstreamName;
		std::wstring ServiceName;
		std::wstring EventName;
		std::wstring EventText;
		std::wstring EventExtText;

		int PhysicalChannel=0;
		std::chrono::system_clock::time_point StartTime;
		std::chrono::system_clock::duration Duration;
	};
}