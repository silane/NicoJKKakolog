#pragma once
#include "Nichan.h"
#include "../MyXml/MyXml.h"
#include <memory>

namespace Nichan
{
	//スレッドのhtmlの解析器
	class Parser
	{
	public:
		virtual Thread Parse(const MyXml::Doc &doc)=0;
	};

	//html5版用パーサー
	class Type1Parser:public Parser
	{
	private:
		static std::chrono::system_clock::time_point GetDate(const std::string &val);
	public:
		virtual Thread Parse(const MyXml::Doc &doc) override;
	};

	//og名前空間を使ってる版用パーサー
	class Type2Parser:public Parser
	{
	private:
		static std::chrono::system_clock::time_point GetDate(const std::string &val);
	public:
		virtual Thread Parse(const MyXml::Doc &doc) override;
	};

	//どのパーサーを使うべきか判定する
	std::unique_ptr<Parser> DetectParser(const MyXml::Doc &doc);

	//ローカルなファイル名でもいい
	Thread ParseFromUrl(const std::string &url);

	Thread PraseFromText(const std::string &text);
}
