#pragma once
#include "Nichan.h"
#include "../MyXml/MyXml.h"
#include <memory>


namespace Nichan
{
	//スレッドのhtmlの解析器
	class ThreadParser
	{
	public:
		virtual Thread Parse(const MyXml::Doc &doc)=0;
		virtual ~ThreadParser()=default;
	};

	//html5版用スレッドパーサー
	class Type1ThreadParser:public ThreadParser
	{
	private:
		static std::chrono::system_clock::time_point GetDate(const std::string &val);
	public:
		virtual Thread Parse(const MyXml::Doc &doc) override;
	};

	//og名前空間を使ってる版用スレッドパーサー
	class Type2ThreadParser:public ThreadParser
	{
	private:
		static std::chrono::system_clock::time_point GetDate(const std::string &val);
	public:
		virtual Thread Parse(const MyXml::Doc &doc) override;
	};

	//板のhtml解析器
	class BoardParser
	{
	public:
		virtual Board Parse(const MyXml::Doc &doc) = 0;
		virtual ~BoardParser() = default;
	};

	//唯一の板パーサー
	class Type1BoardParser:public BoardParser
	{
	public:
		virtual Board Parse(const MyXml::Doc &doc) override;
	};

	//スレの解析でどのパーサーを使うべきか判定する
	std::unique_ptr<ThreadParser> DetectThreadParser(const MyXml::Doc &doc);

	//板の解析でどのパーサーを使うべきか判定する
	std::unique_ptr<BoardParser> DetectBoardParser(const MyXml::Doc &doc);

	//スレのURLからダウンロードしてパースする
	//ローカルなファイル名でもいい
	Thread ParseThreadFromUrl(const std::string &url);

	//スレのHTML文字列からパースする
	Thread ParseThreadFromText(const std::string &text);

	//板のURLからダウンロードしてパースする
	//ローカルなファイル名でもいい
	Board ParseBoardFromUrl(std::string url);

	//板のHTML文字列からパースする
	Board ParseBoardFromText(std::string text);
}
