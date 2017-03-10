#include "../../stdafx.h"
#include "KeywordNichanThreadSelector.h"
#include <libxml\HTMLparser.h>

namespace NicoJKKakolog
{
	KeywordNichanThreadSelector::KeywordNichanThreadSelector(const std::string &threadUrl, const std::vector<std::string> &keywords):
		threadUrl(threadUrl),xpathBaseUrl(new MyXml::XPathExpr(u8"string(/html/head/base/@href)"))
	{
		std::string xpath;
		for (const std::string &keyword : keywords)
		{
			xpath+= u8"contains(../text(),\"" + keyword + u8"\") and ";
		}
		xpath.resize(xpath.size() - 5);

		this->xpathThreads.reset(new MyXml::XPathExpr(u8"/html/body/div[2]/small/a/@href["+xpath+u8"]"));
	}

	std::vector<std::string> KeywordNichanThreadSelector::Get(const ChannelInfo &, const std::chrono::system_clock::time_point &)
	{
		MyXml::Doc doc(htmlReadFile(this->threadUrl.c_str(),nullptr, htmlParserOption::HTML_PARSE_RECOVER));

		std::string baseUrl;
		auto xpathResult = doc.XPath(*this->xpathBaseUrl);
		if (xpathResult.GetString() != "")
			baseUrl = xpathResult.GetString();

		std::vector<std::string> ret;
		xpathResult = doc.XPath(*this->xpathThreads);
		for (int i = 0; i < xpathResult.GetNodeSet()->nodeNr; i++)
		{
			ret.push_back(baseUrl+(char *)xpathResult.GetNodeSet()->nodeTab[i]->children->content);
		}
		
		return ret;
	}
}