#include "../stdafx.h"
#include "IniFile.h"

namespace Utility
{
	IniFile::IniFile(const std::basic_string<TCHAR> &filePath)
	{
		SetFilePath(filePath);
	}

#pragma warning(push)
#pragma warning(disable: 4458)
	void IniFile::SetFilePath(const std::basic_string<TCHAR> &filePath)
	{
		this->filePath = filePath;
	}
#pragma warning(pop)

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
		static const std::basic_regex<TCHAR> reTrue(TEXT("^true|1|enabled?|on|yes|accept$"), std::regex_constants::icase);
		static const std::basic_regex<TCHAR> reFalse(TEXT("^false|0|disabled?|off|no|reject$"), std::regex_constants::icase);

		auto str = GetString(section, key, TEXT(""));

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

	void IniFile::SetString(const std::basic_string <TCHAR> &section, const std::basic_string<TCHAR> &key, const std::basic_string<TCHAR> &val)
	{
		WritePrivateProfileString(section.c_str(),key.c_str(),val.c_str(),filePath.c_str());
	}

	void IniFile::RemoveSection(const std::basic_string<TCHAR> &section)
	{
		WritePrivateProfileString(section.c_str(), NULL,NULL, filePath.c_str());
	}

	void IniFile::RemoveKey(const std::basic_string<TCHAR> &section, const std::basic_string<TCHAR> &key)
	{
		WritePrivateProfileString(section.c_str(), key.c_str(), NULL, filePath.c_str());
	}
}