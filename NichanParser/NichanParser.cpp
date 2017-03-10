#include "NichanParser.h"
#include "Nichan.h"
#include <regex>
#include <ctime>
#include <locale>

extern "C"
{
	#include <libxml\HTMLparser.h>
}

namespace
{
	void TrimSpace(std::string &val)
	{
		std::string::size_type idx;
		for (idx = 0; idx < val.size(); idx++)
		{
			if (!std::isspace(val[idx],std::locale()))
				break;
		}
		val=val.substr(idx);

		if (val.size() <= 1)
			return;

		for (idx = val.size() - 1; idx >= 0; idx--)
		{
			if (!std::isspace(val[idx], std::locale()))
				break;
		}
		val.resize(idx + 1);
	}
}

namespace Nichan
{
	std::chrono::system_clock::time_point Type1ThreadParser::GetDate(const std::string &val)
	{
		static const std::regex regDate(R"(^(\d+)/(\d+)/(\d+)[^ ]* (\d+):(\d+):(\d+)\.(\d+))");

		std::smatch sm;
		if (!std::regex_search(val, sm, regDate))
			return std::chrono::system_clock::time_point();
		
		std::tm hoge;
		hoge.tm_year = std::stoi(sm[1].str())-1900;
		hoge.tm_mon = std::stoi(sm[2].str())-1;
		hoge.tm_mday = std::stoi(sm[3].str());
		hoge.tm_hour = std::stoi(sm[4].str());
		hoge.tm_min = std::stoi(sm[5].str());
		hoge.tm_sec = std::stoi(sm[6].str());

		auto ret=std::chrono::system_clock::from_time_t(std::mktime(&hoge));
		return ret+std::chrono::milliseconds(std::stoi(sm[7].str())*10);
	}

	Thread Type1ThreadParser::Parse(const MyXml::Doc &doc)
	{
		Thread ret;
		ret.url = (char *)doc.GetPtr()->URL;

		MyXml::XPathResult fr = doc.XPath(u8"string(/html/body/h1[@class=\"title\"]/text())");
		ret.title = fr.GetString();
		TrimSpace(ret.title);

		fr=doc.XPath(u8"/html/body/div[@class=\"thread\"]/div[@class=\"post\"]");
		xmlNodeSetPtr nodeSet= fr.GetNodeSet();
		for (int i = 0; i < nodeSet->nodeNr; i++)
		{
			static const MyXml::XPathExpr xPathName(u8"string(div[@class=\"name\"])");
			static const MyXml::XPathExpr xPathMail(u8"string(div[@class=\"name\"]//a/@href)");
			static const MyXml::XPathExpr xPathDate(u8"string(div[@class=\"date\"]/text())");
			static const MyXml::XPathExpr xPathMessage(u8"string(div[@class=\"message\"])");

			Res res;
			xmlChar *str=xmlGetProp(nodeSet->nodeTab[i],BAD_CAST u8"id");
			if (str == NULL)
				continue;
			res.number = std::atoi((char *)str);
			xmlFree(str);

			str = xmlGetProp(nodeSet->nodeTab[i], BAD_CAST u8"data-userid");
			if (str == NULL)
				continue;
			res.id.assign((char *)str+3);
			xmlFree(str);
			
			res.name=doc.XPath(xPathName,nodeSet->nodeTab[i]).GetString();

			res.mail = doc.XPath(xPathMail,nodeSet->nodeTab[i]).GetString();
			if(res.mail.size()>7)
				res.mail = res.mail.substr(7);

			res.date = GetDate(doc.XPath(xPathDate, nodeSet->nodeTab[i]).GetString());

			res.message = doc.XPath(xPathMessage, nodeSet->nodeTab[i]).GetString();
			TrimSpace(res.message);

			ret.res.push_back(std::move(res));
		}

		return ret;
	}

	std::chrono::system_clock::time_point Type2ThreadParser::GetDate(const std::string &val)
	{
		static const std::regex regDate(R"((\d+)/(\d+)/(\d+)[^ ]* (\d+):(\d+):(\d+)\.(\d+))");

		std::smatch sm;
		if (!std::regex_search(val, sm, regDate))
			return std::chrono::system_clock::time_point();

		std::tm hoge;
		hoge.tm_year = std::stoi(sm[1].str()) - 1900;
		hoge.tm_mon = std::stoi(sm[2].str()) - 1;
		hoge.tm_mday = std::stoi(sm[3].str());
		hoge.tm_hour = std::stoi(sm[4].str());
		hoge.tm_min = std::stoi(sm[5].str());
		hoge.tm_sec = std::stoi(sm[6].str());

		auto ret = std::chrono::system_clock::from_time_t(std::mktime(&hoge));
		return ret + std::chrono::milliseconds(std::stoi(sm[7].str()) * 10);
	}

	Thread Type2ThreadParser::Parse(const MyXml::Doc &doc)
	{
		Thread ret;
		ret.url = (char *)doc.GetPtr()->URL;

		MyXml::XPathResult fr = doc.XPath(u8"string(/html/body/div/span/h1/text())");
		ret.title = fr.GetString();
		TrimSpace(ret.title);

		fr = doc.XPath(u8"//dl[@class=\"thread\"]/dt");
		xmlNodeSetPtr nodeSet = fr.GetNodeSet();
		for (int i = 0; i < nodeSet->nodeNr; i++)
		{
			static const MyXml::XPathExpr xpathNumber(u8"number(substring-before(text()[1],' '))");
			static const MyXml::XPathExpr xpathId(u8"string(text()[last()])");
			static const MyXml::XPathExpr xpathName(u8"string(//b/text())");
			static const MyXml::XPathExpr xpathMail(u8"string(a/@href)");
			static const MyXml::XPathExpr xpathMessage(u8"string(following-sibling::dd[1])");

			Res res;
			
			res.number = (int)doc.XPath(xpathNumber,nodeSet->nodeTab[i]).GetFloat();

			std::string hoge = doc.XPath(xpathId, nodeSet->nodeTab[i]).GetString();
			std::string::size_type idx = hoge.find(u8"ID:");
			res.id = (idx != std::string::npos) ? hoge.substr(idx + 3) : u8"";
			
			res.name = doc.XPath(xpathName, nodeSet->nodeTab[i]).GetString();

			res.mail = doc.XPath(xpathMail, nodeSet->nodeTab[i]).GetString();
			if (res.mail.size()>7)
				res.mail = res.mail.substr(7);

			res.date = GetDate(hoge);

			res.message = doc.XPath(xpathMessage, nodeSet->nodeTab[i]).GetString();
			TrimSpace(res.message);

			ret.res.push_back(std::move(res));
		}

		return ret;
	}

	std::unique_ptr<ThreadParser> DetectThreadParser(const MyXml::Doc &doc)
	{
		if (doc.XPath(u8"boolean(/html/@prefix)").GetBool())
			return std::unique_ptr<ThreadParser>(new Type2ThreadParser());
		else
			return std::unique_ptr<ThreadParser>(new Type1ThreadParser());
	}

	Board Type1BoardParser::Parse(const MyXml::Doc &doc)
	{
		static const MyXml::XPathExpr xpathTitle(u8"string(/html/head/title/text())");
		static const MyXml::XPathExpr xpathBaseUrl(u8"string(/html/head/base/@href)");
		static const MyXml::XPathExpr xpathThreads(u8"/html/body/div[2]/small/a");

		Board ret;
		ret.url = (char *)doc.GetPtr()->URL;

		ret.title = doc.XPath(xpathTitle).GetString();
		auto idx = ret.title.find(u8"Åó");
		if(idx!=std::string::npos)
			ret.title.resize(idx);
		
		std::string baseUrl = doc.XPath(xpathBaseUrl).GetString();
		if (baseUrl.size() == 0)
		{
			baseUrl = ret.url;
		}

		MyXml::XPathResult xpathResult = doc.XPath(xpathThreads);
		for (int i = 0; i < xpathResult.GetNodeSet()->nodeNr; i++)
		{
			Thread thread;
			thread.url = baseUrl + (char *)xmlGetProp(xpathResult.GetNodeSet()->nodeTab[i], BAD_CAST "href");
			thread.title = (char *)xpathResult.GetNodeSet()->nodeTab[i]->children->content+3;

			ret.thread.push_back(std::move(thread));
		}

		return ret;
	}

	std::unique_ptr<BoardParser> DetectBoardParser(const MyXml::Doc &)
	{
		return std::unique_ptr<BoardParser>(new Type1BoardParser());
	}

	Thread ParseThreadFromUrl(const std::string &url)
	{
		htmlDocPtr docPtr=htmlReadFile(url.c_str(), NULL, htmlParserOption::HTML_PARSE_RECOVER);
		if (!docPtr)
		{
			throw std::runtime_error("Couldn't download file or couldn't parse html");
		}

		MyXml::Doc doc(docPtr);
		auto parser = DetectThreadParser(doc);
		return parser->Parse(doc);
	}

	Thread ParseThreadFromText(const std::string &text)
	{
		htmlDocPtr docPtr = htmlReadMemory(text.c_str(), (int)text.size(), "", NULL, htmlParserOption::HTML_PARSE_RECOVER);
		if (!docPtr)
		{
			throw std::runtime_error("Couldn't parse html");
		}

		MyXml::Doc doc(docPtr);
		auto parser = DetectThreadParser(doc);
		return parser->Parse(doc);
	}

	Board ParseBoardFromUrl(std::string url)
	{
		htmlDocPtr docPtr = htmlReadFile(url.c_str(), NULL, htmlParserOption::HTML_PARSE_RECOVER);
		if (!docPtr)
		{
			throw std::runtime_error("Couldn't download file or couldn't parse html");
		}

		MyXml::Doc doc(docPtr);
		auto parser = DetectBoardParser(doc);
		return parser->Parse(doc);
	}

	Board ParseBoardFromText(std::string text)
	{
		htmlDocPtr docPtr = htmlReadMemory(text.c_str(), (int)text.size(), "", NULL, htmlParserOption::HTML_PARSE_RECOVER);
		if (!docPtr)
		{
			throw std::runtime_error("Couldn't parse html");
		}

		MyXml::Doc doc(docPtr);
		auto parser = DetectBoardParser(doc);
		return parser->Parse(doc);
	}
}