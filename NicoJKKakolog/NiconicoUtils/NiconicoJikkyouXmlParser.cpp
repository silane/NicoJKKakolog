#include "../../stdafx.h"
#include "NiconicoJikkyouXmlParser.h"

namespace NicoJKKakolog{

	const NiconicoJikkyouXmlParser::ColorName NiconicoJikkyouXmlParser::colorNameMapping[]= {
		{ RGB(0xFF, 0x00, 0x00), "red" },
		{ RGB(0xFF, 0x80, 0x80), "pink" },
		{ RGB(0xFF, 0xC0, 0x00), "orange" },
		{ RGB(0xFF, 0xFF, 0x00), "yellow" },
		{ RGB(0x00, 0xFF, 0x00), "green" },
		{ RGB(0x00, 0xFF, 0xFF), "cyan" },
		{ RGB(0x00, 0x00, 0xFF), "blue" },
		{ RGB(0xC0, 0x00, 0xFF), "purple" },
		{ RGB(0x00, 0x00, 0x00), "black" },
		{ RGB(0xCC, 0xCC, 0x99), "white2" },
		{ RGB(0xCC, 0xCC, 0x99), "niconicowhite" },
		{ RGB(0xCC, 0x00, 0x33), "red2" },
		{ RGB(0xCC, 0x00, 0x33), "truered" },
		{ RGB(0xFF, 0x33, 0xCC), "pink2" },
		{ RGB(0xFF, 0x66, 0x00), "orange2" },
		{ RGB(0xFF, 0x66, 0x00), "passionorange" },
		{ RGB(0x99, 0x99, 0x00), "yellow2" },
		{ RGB(0x99, 0x99, 0x00), "madyellow" },
		{ RGB(0x00, 0xCC, 0x66), "green2" },
		{ RGB(0x00, 0xCC, 0x66), "elementalgreen" },
		{ RGB(0x00, 0xCC, 0xCC), "cyan2" },
		{ RGB(0x33, 0x99, 0xFF), "blue2" },
		{ RGB(0x33, 0x99, 0xFF), "marineblue" },
		{ RGB(0x66, 0x33, 0xCC), "purple2" },
		{ RGB(0x66, 0x33, 0xCC), "nobleviolet" },
		{ RGB(0x66, 0x66, 0x66), "black2" },
	};

NiconicoJikkyouXmlParser::NiconicoJikkyouXmlParser(bool socketFormat):
	socketFormat(socketFormat),inChatTag(false)
{
}


NiconicoJikkyouXmlParser::~NiconicoJikkyouXmlParser()
{
}

void NiconicoJikkyouXmlParser::PushString(const std::string &str)
{
	buffer += str;

	if (socketFormat)
	{
		for (std::size_t idx = buffer.find('\0'); idx != std::string::npos; idx = buffer.find('\0'))
		{
			std::string chatTag = buffer.substr(0, idx);
			if (chatTag.compare(0, 5, "<chat") == 0)
			{
				try {
					chats.push(std::make_pair(GetTimeFromChatTag(chatTag),GetChatFromChatTag(chatTag)));
				}
				catch (...) {}
			}
			buffer = buffer.substr(idx + 1);
		}
	}
	else
	{
		while (true)
		{
			if (!inChatTag)
			{
				std::string::size_type idx = buffer.find("<chat ");
				if (idx == std::string::npos)
					break;
				buffer = buffer.substr(idx);
				inChatTag = true;
			}
			else
			{
				std::string::size_type endIdx = buffer.find("/chat>");
				if (endIdx == std::string::npos)
					break;

				endIdx += 6;
				try {
					std::string chatTag = buffer.substr(0, endIdx);
					chats.push(std::make_pair(GetTimeFromChatTag(chatTag),GetChatFromChatTag(chatTag)));
				}
				catch (...) {}
				buffer = buffer.substr(endIdx);
				inChatTag = false;
			}
		}
	}
}

std::pair<time_t,Chat> NiconicoJikkyouXmlParser::PopChat() {
	auto ret=chats.front();
	chats.pop();
	return ret;
}

bool NiconicoJikkyouXmlParser::HasChat() {
	return chats.size() > 0;
}

void NiconicoJikkyouXmlParser::Reset() {
	inChatTag = false;
	buffer.clear();
	chats = std::queue<std::pair<std::time_t,Chat>>();
}

Chat NiconicoJikkyouXmlParser::GetChatFromChatTag(const std::string &str)
{
	static const std::regex reChat("<chat(?= )(.*)>(.*?)</chat>");
	static const std::regex reMail(" mail=\"(.*?)\"");
	static const std::regex reAbone(" abone=\"1\"");
	static const std::regex reUserID(" user_id=\"([0-9A-Za-z\\-_]{0,27})");
	static const std::regex reNo(" no=\"(\\d+)\"");
	static const std::regex reColor("(?:^| )#([0-9A-Fa-f]{6})(?: |$)");

	Chat ret;
	ret.color = RGB(255, 255, 255);
	std::smatch sm;
	std::regex_match(str, sm, reChat);
	ret.text = sm.str(2);

	std::regex_search(str, sm, reUserID);
	ret.userId = sm.str(1);

	std::regex_search(str, sm, reNo);
	ret.number = std::stoi(sm.str(1));

	if (std::regex_search(str, sm, reMail))
	{
		std::string mail = sm.str(1);

		if (mail.find("shita") != std::string::npos) {
			ret.position = Chat::Position::Down;
		}
		else if (mail.find("ue") != std::string::npos) {
			ret.position = Chat::Position::Up;
		}
		else {
			ret.position = Chat::Position::Default;
		}

		if (mail.find("small") != std::string::npos) {
			ret.size = Chat::Size::Small;
		}
		else {
			ret.size = Chat::Size::Default;
		}

		ret.color = RGB(0xFF, 0xFF, 0xFF);;
		if (std::regex_search(mail, sm, reColor)) {
			int color = std::stoi(sm.str(1), 0, 16);
			ret.color = RGB((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
		}
		else {
			for (int i = 0; i < sizeof(colorNameMapping)/sizeof(ColorName); ++i) {
				if (mail.find(colorNameMapping[i].name) != std::string::npos) {
					ret.color = colorNameMapping[i].color;
					break;
				}
			}
			//colorNameMapping.at()
			//ret.color = RGB(0xFF, 0xFF, 0xFF);
		}
	}

	return ret;
}

std::time_t NiconicoJikkyouXmlParser::GetTimeFromChatTag(const std::string &str)
{
	static const std::regex reDate("date=\"(\\d+)\"");

	std::smatch sm;
	std::regex_search(str, sm, reDate);
	return (std::time_t)std::stoull(sm.str(1));
}
}