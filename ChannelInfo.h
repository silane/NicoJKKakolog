#pragma once
#include <cstdint>
#include <string>
#include <chrono>

namespace NicoJKKakolog
{
	//ChatProvider��Get�ɓn�����`�����l�����
	//���ׂĂ����܂��Ă���Ƃ͌���Ȃ��B���ɘ^��Đ����B
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