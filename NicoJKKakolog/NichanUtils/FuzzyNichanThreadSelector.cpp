#include "../../stdafx.h"
#include "FuzzyNichanThreadSelector.h"
#include "../../NichanParser/NichanParser.h"
#include <algorithm>
#include <utility>
#include <regex>

#undef min//win32APIのマクロ回避
#undef max

namespace NicoJKKakolog
{
	int FuzzyNichanThreadSelector::LevenshteinDistance(const std::string &str1, const std::string &str2)
	{
		std::vector<std::vector<int>> d(str1.size()+1);
		for (decltype(d)::size_type i = 0; i <= str1.size(); i++) 
		{
			d[i].resize(str2.size() + 1);
			d[i][0] = (int)i;
		}
		for (decltype(d)::size_type i = 0; i <= str2.size(); i++)
		{
			d[0][i] = (int)i;
		}

		for (decltype(d)::size_type i1 = 1; i1 <= str1.size(); i1++)
			for (decltype(d)::size_type i2 = 1; i2 <= str2.size(); i2++)
			{
				const int cost = (str1[i1] == str2[i2]) ? 0 : 1;
				d[i1][i2] = std::min({ d[i1 - 1][i2] + 1,d[i1][i2 - 1] + 1,d[i1 - 1][i2 - 1] + cost });
			}
		return d[str1.size()][str2.size()];
	}

	void FuzzyNichanThreadSelector::NormalizeThreadTitle(std::string &text)
	{
		static const std::regex reResNum(u8R"(\(%d+\)$)");
		static const std::regex reBracket(u8R"((【.*】))");
		text = std::regex_replace(text, reResNum, u8"");
		text = std::regex_replace(text, reBracket, u8"");

		const char mudantensai[] = u8"無断転載禁止";
		const char copyright[] = u8"©";
		const char nichandotnet[] = u8"2ch.net";
		const char copyright2[] = u8"&#169;";
		std::string::size_type idx = text.find(mudantensai);
		if (idx != std::string::npos)
			text = text.replace(idx, sizeof(mudantensai) - 1, u8"");
		idx = text.find(copyright);
		if (idx != std::string::npos)
			text = text.replace(idx, sizeof(copyright) - 1, u8"");
		idx = text.find(nichandotnet);
		if (idx != std::string::npos)
			text = text.replace(idx, sizeof(nichandotnet) - 1, u8"");
		idx = text.find(copyright2);
		if (idx != std::string::npos)
			text = text.replace(idx, sizeof(copyright2) - 1, u8"");
	}

	FuzzyNichanThreadSelector::FuzzyNichanThreadSelector(const std::string &boardUrl, const std::string &threadTitle):
		boardUrl(boardUrl),threadTitle(threadTitle)
	{
		NormalizeThreadTitle(this->threadTitle);
	}


	std::vector<std::string> FuzzyNichanThreadSelector::Get(const ChannelInfo &channel, const std::chrono::system_clock::time_point &t)
	{
		auto board = Nichan::ParseBoardFromUrl(this->boardUrl);

		//1000まで埋まっているスレを除外
		for (auto itr = std::begin(board.thread); itr != std::end(board.thread);)
		{
			if (itr->title.compare(itr->title.size() - 6, 6, u8"(1001)") == 0)
				itr = board.thread.erase(itr);
			else
				++itr;
		}

		typedef std::pair<Nichan::Thread *, int> item_type;
		std::vector<item_type> distances(board.thread.size());

		//レーベンシュタイン距離をそれぞれ計算
		std::transform(std::begin(board.thread), std::end(board.thread), std::begin(distances),
			[this](Nichan::Thread &th) {

			NormalizeThreadTitle(th.title);//boardの内容を変更してるので注意！
			return item_type(&th, LevenshteinDistance(th.title, this->threadTitle)); 
		});

		std::sort(std::begin(distances), std::end(distances),
			[](const item_type &item1, const item_type &item2) {return item1.second < item2.second; });
		
		
		if (distances.size() == 0)
			return{};

		//レーベンシュタイン距離の最小値から距離30以内のものを選択
		std::vector<std::string> ret;
		int min_distance = distances[0].second;
		for (const auto &item : distances)
		{
			if (item.second - min_distance >= 30)
				break;
			ret.push_back(item.first->url);
		}
		
		return ret;
	}
}