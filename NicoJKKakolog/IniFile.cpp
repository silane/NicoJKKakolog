#include "../stdafx.h"
#include "IniFile.h"

namespace Utility
{
	IniFile::IniFile(const std::basic_string<TCHAR> &filePath)
	{
		SetFilePath(filePath);
	}

	void IniFile::SetFilePath(const std::basic_string<TCHAR> &filePath)
	{
		this->filePath = filePath;
	}

	int IniFile::GetInt(const std::basic_string<TCHAR> &section, const std::basic_string<TCHAR> &key, int defaultVal) const
	{
		return (int)GetPrivateProfileInt(section.c_str(), key.c_str(), defaultVal, this->filePath.c_str());
	}

	std::basic_string<TCHAR> IniFile::GetString(const std::basic_string<TCHAR> &section, const std::basic_string<TCHAR> &key, const std::basic_string<TCHAR> &defaultVal) const
	{
		constexpr int RET_SIZE = 4096;
		TCHAR ret[RET_SIZE];
		auto size = GetPrivateProfileString(section.c_str(), key.c_str(), defaultVal.c_str(), ret, RET_SIZE, this->filePath.c_str());
		return std::basic_string<TCHAR>(ret, size);
	}

	bool IniFile::GetBool(const std::basic_string<TCHAR> &section, const std::basic_string<TCHAR> &key, bool defaultVal) const
	{
		static const std::basic_regex<TCHAR> reTrue(TEXT("^true$"), std::regex_constants::icase);
		static const std::basic_regex<TCHAR> reFalse(TEXT("^false$"), std::regex_constants::icase);

		auto str = GetString(section, key, TEXT(""));

		try {
			std::basic_string<TCHAR>::size_type idx;
			int ret = std::stoi(str, &idx);
			if (idx == str.size())
				return ret;
		}
		catch (...) {}

		if (std::regex_match(str, reTrue))
			return true;
		else if (std::regex_match(str, reFalse))
			return false;

		return defaultVal;
	}

	std::vector < std::basic_string<TCHAR>> IniFile::GetSections() const
	{
		constexpr int RET_SIZE = 4096;
		std::vector < std::basic_string<TCHAR>> sections;
		TCHAR ret[RET_SIZE];
		auto size = GetPrivateProfileSectionNames(ret, RET_SIZE, filePath.c_str());
		std::basic_string<TCHAR> str(ret, size);
		for (auto idx = str.find(TEXT("\0")); idx != std::basic_string<TCHAR>::npos; idx = str.find(TEXT("\0")))
		{
			sections.push_back(str.substr(0, idx));
			str = str.substr(idx + 1);
		}
		return sections;
	}

	std::unordered_map<std::basic_string<TCHAR>, std::basic_string<TCHAR>> IniFile::GetSectionContent(const std::basic_string<TCHAR> &section) const
	{
		constexpr int RET_SIZE = 4096;
		std::unordered_map < std::basic_string<TCHAR>, std::basic_string<TCHAR>> map;
		TCHAR ret[RET_SIZE];
		auto size = GetPrivateProfileSection(section.c_str(), ret, RET_SIZE, filePath.c_str());
		if (size <= 0)
			return map;
		std::basic_string<TCHAR> str(ret, size);
		for (auto idx = str.find(TEXT('\0')); idx!=std::basic_string<TCHAR>::npos&&idx!=0; idx = str.find(TEXT('\0')))
		{
			auto hoge = str.substr(0, idx);
			auto eqIdx = hoge.find(TEXT("="));
			map[hoge.substr(0, eqIdx)] = hoge.substr(eqIdx + 1);
			str = str.substr(idx + 1);
		}
		return map;
	}	
}