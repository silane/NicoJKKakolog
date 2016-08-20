#pragma once
#include "INichanThreadSelector.h"

namespace NicoJKKakolog
{
	class FuzzyNichanThreadSelector :
		public INichanThreadSelector
	{
	private:
		std::string boardUrl;
		std::string threadTitle;
		
		static int LevenshteinDistance(const std::string &str1, const std::string &str2);
		static void NormalizeThreadTitle(std::string &text);

	public:
		FuzzyNichanThreadSelector(const std::string &boardUrl,const std::string &threadTitle);
		virtual std::vector<std::string> Get(const ChannelInfo &channel, const std::chrono::system_clock::time_point &t) override;
	};
}