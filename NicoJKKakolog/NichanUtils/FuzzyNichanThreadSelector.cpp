#include "../../stdafx.h"
#include "FuzzyNichanThreadSelector.h"
#include "../../NichanParser/NichanParser.h"
#include <limits>
#include <algorithm>

#undef min//win32API‚Ìƒ}ƒNƒ‰ñ”ğ
#undef max

namespace NicoJKKakolog
{
	int FuzzyNichanThreadSelector::LevenshteinDistance(const std::string &str1, const std::string &str2)
	{
		std::vector<std::vector<int>> d(str1.size()+1);
		for (int i = 0; i <= str1.size(); i++) 
		{
			d[i].resize(str2.size() + 1);
			d[i][0] = i;
		}
		for (int i = 0; i <= str2.size(); i++)
		{
			d[0][i] = i;
		}

		for (int i1 = 1; i1 <= str1.size(); i1++)
			for (int i2 = 1; i2 <= str2.size(); i2++)
			{
				const int cost = (str1[i1] == str2[i2]) ? 0 : 1;
				d[i1][i2] = std::min({ d[i1 - 1][i2] + 1,d[i1][i2 - 1] + 1,d[i1 - 1][i2 - 1] + cost });
			}
		return d[str1.size()][str2.size()];
	}

	FuzzyNichanThreadSelector::FuzzyNichanThreadSelector(const std::string &boardUrl, const std::string &threadTitle):
		boardUrl(boardUrl),threadTitle(threadTitle)
	{
	}


	std::vector<std::string> FuzzyNichanThreadSelector::Get(const ChannelInfo &channel, const std::chrono::system_clock::time_point &t)
	{
		auto board = Nichan::ParseBoardFromUrl(this->boardUrl);
		std::vector<int> distances(board.thread.size());

		std::transform(std::begin(board.thread), std::end(board.thread), std::begin(distances),
			[this](const Nichan::Thread &th) {return LevenshteinDistance(th.title, this->threadTitle); });

		int min_distance=0;
		auto itr=std::min(std::begin(distances), std::end(distances));
		if (itr != std::end(distances))
			min_distance = *itr;

		std::vector<std::string> ret;
		for (int i=0;i<distances.size();i++)
		{
			if (distances[i] - min_distance<=6)
				ret.push_back(board.thread[i].url);
		}

		return ret;
	}
}