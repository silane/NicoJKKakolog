#pragma once
#include <string>
#include <chrono>
#include <vector>

namespace Nichan
{
	struct Res
	{
		int number;
		std::string name;
		std::string mail;
		std::string id;
		std::chrono::system_clock::time_point date;
		std::string message;
	};

	struct Thread
	{
		std::string url;
		std::string title;
		std::vector<Res> res;
	};

	struct Board
	{
		std::string url;
		std::string title;
		std::vector<Thread> thread;
	};
}
