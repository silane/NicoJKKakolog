#pragma once
#include "../../MyXml/MyXml.h"
#include "INichanThreadSelector.h"
#include <vector>
#include <memory>

namespace NicoJKKakolog
{
	class KeywordNichanThreadSelector :public INichanThreadSelector
	{
	private:
		std::string threadUrl;
		
		std::unique_ptr<MyXml::XPathExpr> xpathBaseUrl;
		std::unique_ptr<MyXml::XPathExpr> xpathThreads;

	public:
		KeywordNichanThreadSelector(const std::string &threadUrl, const std::vector<std::string> &keywords);
		virtual std::vector<std::string> Get(const ChannelInfo &channel, const std::chrono::system_clock::time_point &t);
		virtual ~KeywordNichanThreadSelector() = default;
	};
}
