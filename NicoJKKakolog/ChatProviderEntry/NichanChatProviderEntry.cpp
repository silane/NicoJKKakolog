#include "../../stdafx.h"
#include "NichanChatProviderEntry.h"
#include "../ChatProvider/NichanChatProvider.h"
#include "../NichanUtils/KeywordNichanThreadSelector.h"
#include "../NichanChatProviderSettingDialog.h"

namespace NicoJKKakolog
{
	NichanChatProviderEntry::NichanChatProviderEntry(HINSTANCE hInstance,Utility::IniFile *iniFile):
		hInstance(hInstance),iniFile(iniFile)
	{
	}

	std::wstring NichanChatProviderEntry::GetName() const
	{
		return L"2chスレ表示";
	}

	std::wstring NichanChatProviderEntry::GetDescription() const
	{
		return L"2chのスレからコメントを表示";
	}

	IChatProvider *NichanChatProviderEntry::NewProvider()
	{
		Color color;
		{
			auto colorStr = iniFile->GetString(_T("NicoJKKakolog"), _T("2chChatColor"), _T(""));
			if (colorStr.size() == 0)
				color = ColorNone;
			else
			try
			{
				unsigned char r = (unsigned char)std::stoi(colorStr);
				auto idx = colorStr.find(_T(','));
				unsigned char g = (unsigned char)std::stoi(colorStr.substr(idx+1));
				idx = colorStr.find(_T(','), idx+1);
				unsigned char b = (unsigned char)std::stoi(colorStr.substr(idx+1));
				color = RGB(r, g, b);
			}
			catch (...)
			{
				color = RGB(0, 0, 0);
			}
		}

		NichanChatProviderSettingDialog dialog(hInstance, iniFile->GetSectionContent(L"2chBoards"));
		std::unique_ptr<INichanThreadSelector> selector=dialog.Show();
		if (!selector)
			return nullptr;


		auto resCollectInterval = std::chrono::milliseconds(iniFile->GetInt(_T("NicoJKKakolog"), _T("ResCollectInterval"), 1000));
		if (resCollectInterval < std::chrono::milliseconds(1000))
			resCollectInterval = std::chrono::milliseconds(1000);
		return new NichanChatProvider(color,resCollectInterval ,
			std::chrono::milliseconds(iniFile->GetInt(_T("NicoJKKakolog"), _T("ThreSearchInterval"), 60000)),
			selector.release());
	}

	void NichanChatProviderEntry::DeleteProvider(std::unique_ptr<IChatProvider> provider) {}
}