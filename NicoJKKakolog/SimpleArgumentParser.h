#pragma once
#include <string>
#include <vector>

namespace Utility
{
	class SimpleArgumentParser
	{
	private:
		std::wstring prefix_chars;
		std::vector<std::wstring> args;
	public:
		SimpleArgumentParser(const std::wstring &args = GetCommandLineW(), const std::wstring &prefix_chars = L"-/");

		bool GetOptionFlags(const std::wstring &name);
		std::wstring GetOptionArgument(const std::wstring &name);
	};
}