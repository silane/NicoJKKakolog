#include "stdafx.h"
#include "NiconicoJikkyouLogChatProviderEntry.h"
#include "NiconicoJikkyouLogChatProvider.h"
#include "DefaultJkIdTable.h"
#include <codecvt>

namespace NicoJKKakolog
{
	NiconicoJikkyouLogChatProviderEntry::NiconicoJikkyouLogChatProviderEntry(Utility::IniFile *iniFile):
		jkIdTable(GetDefaultJkIdTable()),iniFile(iniFile)
	{
		for (auto &pair : iniFile->GetSectionContent(TEXT("Channels")))
		{
			try {
				uint_least32_t serviceId=(uint_least32_t)std::stol(pair.first, nullptr, 0);
				serviceId = serviceId >> 16 | serviceId << 16;//なぜか逆になっているらしい
				jkIdTable[serviceId] = std::stoi(pair.second);
			}
			catch (const std::invalid_argument &e) {}
		}
	}


	NiconicoJikkyouLogChatProviderEntry::~NiconicoJikkyouLogChatProviderEntry()
	{
	}

	std::string NiconicoJikkyouLogChatProviderEntry::GetName() const
	{
		return u8"ニコニコ実況過去ログ";
	}
	std::string NiconicoJikkyouLogChatProviderEntry::GetDescription() const
	{
		return u8"自動で過去ログを表示する";
	}
	IChatProvider * NiconicoJikkyouLogChatProviderEntry::NewProvider()
	{
		static std::wstring_convert<std::codecvt_utf8<TCHAR>, TCHAR> cvt;
		auto userid = iniFile->GetString(TEXT("NicoJKKakolog"), TEXT("niconicouserid"), TEXT(""));
		auto password = iniFile->GetString(TEXT("NicoJKKakolog"), TEXT("niconicopassword"), TEXT(""));
		if (userid == TEXT("") || password == TEXT(""))
		{
			MessageBox(nullptr, TEXT("ニコニコ動画のログイン情報がiniファイルに設定されていません"), TEXT(""), 0);
			return nullptr;
		}
		
		login.Login(cvt.to_bytes(userid), cvt.to_bytes(password));

		if (!login.IsLoggedIn())
		{
			MessageBox(nullptr, TEXT("ニコニコ動画にログインできませんでした"),TEXT(""),0);
			return nullptr;
		}

		NiconicoJikkyouLogChatProvider *ret = new NiconicoJikkyouLogChatProvider(jkIdTable,&login);
		return ret;
	}
	void NiconicoJikkyouLogChatProviderEntry::DeleteProvider(std::unique_ptr<IChatProvider> provider)
	{
	}
}