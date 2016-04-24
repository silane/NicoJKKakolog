#pragma once
#include <string>
#include <unordered_map>
#include <regex>
#include <Windows.h>

namespace Utility 
{
	class IniFile
	{
	private:
		std::basic_string<TCHAR> filePath;

	public:
		IniFile() = default;
		explicit IniFile(const std::basic_string<TCHAR> &filePath);
		void SetFilePath(const std::basic_string<TCHAR> &filePath);

		int GetInt(const std::basic_string<TCHAR> &section, const std::basic_string<TCHAR> &key, int defaultVal) const;
		std::basic_string<TCHAR> GetString(const std::basic_string<TCHAR> &section, const std::basic_string<TCHAR> &key, const std::basic_string<TCHAR> &defaultVal) const;
		bool GetBool(const std::basic_string<TCHAR> &section, const std::basic_string<TCHAR> &key, bool defaultVal) const;
		std::vector < std::basic_string<TCHAR>> GetSections() const;
		std::unordered_map<std::basic_string<TCHAR>, std::basic_string<TCHAR>> GetSectionContent(const std::basic_string<TCHAR> &section) const;
	};
}