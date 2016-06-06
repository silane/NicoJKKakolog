#include "../../stdafx.h"
#include "NicoJikkyouLogFileStreamChatProvider.h"

namespace NicoJKKakolog
{
	NicoJikkyouLogFileStreamChatProvider::NicoJikkyouLogFileStreamChatProvider(const std::string &path):
		ifs(path)
	{
		SetStream(ifs);
	}


	NicoJikkyouLogFileStreamChatProvider::~NicoJikkyouLogFileStreamChatProvider()
	{
	}
}