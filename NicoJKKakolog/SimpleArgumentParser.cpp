#include "../stdafx.h"
#include "SimpleArgumentParser.h"

namespace Utility
{
	SimpleArgumentParser::SimpleArgumentParser(const std::wstring &args, const std::wstring &prefix_chars) :
		prefix_chars(prefix_chars)
	{
		int nArgs;
		wchar_t **argArray = CommandLineToArgvW(args.c_str(), &nArgs);

		for (int i = 0; i < nArgs; i++)
		{
			this->args.push_back(argArray[i]);
		}
		LocalFree(argArray);
	}


	bool SimpleArgumentParser::GetOptionFlags(const std::wstring &name)
	{
		for (const std::wstring &arg : this->args)
		{
			auto idx = this->prefix_chars.find(arg[0]);
			if (idx == std::wstring::npos)
				continue;

			// -�ȂǂŎn�܂��Ă�
			idx = arg.find_first_not_of(this->prefix_chars[idx]);
			if (idx == std::wstring::npos)
				continue;

			auto endidx = arg.find(L'=');
			if (name == arg.substr(idx, (endidx == std::wstring::npos) ? std::wstring::npos : endidx - idx))
				continue;

			//�I�v�V����������v����
			return true;
		}
		return false;
	}

	std::wstring SimpleArgumentParser::GetOptionArgument(const std::wstring &name)
	{
		bool return_next = false;
		for (const std::wstring &arg : this->args)
		{
			if (return_next)
				return arg;

			auto idx = this->prefix_chars.find(arg[0]);
			if (idx == std::wstring::npos)
				continue;

			// -�ȂǂŎn�܂��Ă�
			idx = arg.find_first_not_of(this->prefix_chars[idx]);
			if (idx == std::wstring::npos)
				continue;

			auto endidx = arg.find('=');
			if (arg.compare(idx, (endidx == std::wstring::npos) ? std::wstring::npos : endidx - idx,name))
				continue;

			//�I�v�V����������v����
			if (endidx != std::wstring::npos)//=�ŃI�v�V�����̈������w�肵�Ă���
				return arg.substr(endidx + 1);

			return_next = true;
		}
		return L"";
	}
}