#pragma once

#include <string>

namespace NicoJKKakolog
{
	typedef COLORREF Color;

class Chat
{
public:
	enum class Position {Default,Up,Down};
	enum class Size {Default,Small};

	int number=0;
	std::string text;
	std::string userId;
	Position position=Position::Default;
	Size size=Size::Default;
	Color color=0;
	
	Chat() = default;
	Chat( int number,const std::string &text,const std::string &userId, Color color, Position position=Position::Default,Size size=Size::Default) noexcept;
};
}