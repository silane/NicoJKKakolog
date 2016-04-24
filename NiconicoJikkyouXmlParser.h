#pragma once
#include "Chat.h"
#include <queue>
#include <unordered_map>
#include <utility>

namespace NicoJKKakolog
{
	class NiconicoJikkyouXmlParser
	{
	private:
		struct ColorName { Color color; char *name; };
		static const ColorName colorNameMapping[];

		//static const std::unordered_map<std::string, Color> colorNameMapping;

		bool socketFormat;
		std::queue < std::pair<std::time_t, Chat >> chats;
		std::string buffer;
		bool inChatTag;

	public:
		explicit NiconicoJikkyouXmlParser(bool socketFormat=false);
		~NiconicoJikkyouXmlParser();

		void PushString(const std::string &str);
		std::pair<std::time_t, Chat> PopChat();
		bool HasChat();
		void Reset();

		static Chat GetChatFromChatTag(const std::string &str);
		static std::time_t GetTimeFromChatTag(const std::string &str);
	};
}