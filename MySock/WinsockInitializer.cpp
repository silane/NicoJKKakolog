#include "WinsockInitializer.h"
#include <stdexcept>
#include "CommonIncludes.h"

namespace MySock
{
	WinsockInitializer::WinsockInitializer()
	{
#ifdef _WIN32
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) == 0)
			return;

		throw std::runtime_error("failed to initialize winsock");
#endif
	}


	WinsockInitializer::~WinsockInitializer()
	{
#ifdef _WIN32
		WSACleanup();
#endif
	}
}