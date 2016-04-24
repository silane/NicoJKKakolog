#pragma once
#include <memory>
#include "IChatProvider.h"

namespace NicoJKKakolog
{
	//NicoJKKakologに登録しChatProviderを返す
	class IChatProviderEntry
	{
	public:
		//ユーザーに表示するエントリの名前
		virtual std::string GetName() const=0;
		//ユーザーに表示するエントリの説明
		virtual std::string GetDescription() const = 0;
		//ユーザーがエントリを有効にしたときに呼ばれる。ChatProviderを返すようにする。ユーザーへのオプション提示はここでするのが良い。
		virtual IChatProvider *NewProvider() = 0;
		//現在呼ばれることはないです
		virtual void DeleteProvider(std::unique_ptr<IChatProvider> provider) = 0;
		virtual ~IChatProviderEntry() = default;
	};

}