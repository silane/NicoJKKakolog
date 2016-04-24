#include "stdafx.h"
#include "Chat.h"
namespace NicoJKKakolog {
	Chat::Chat(int number, const std::string &text, const std::string &userId, Color color, Position position, Size size) noexcept:
		number(number),text(text),userId(userId),position(position),size(size),color(color)
	{
	}
}