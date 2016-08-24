#pragma once

#include <string>
#include <vector>
#include <system_error>
#include "IPAddress.h"

namespace MySock
{
	class Dns
	{
	public:
		Dns() = delete;

		static std::vector<IPAddress> GetHostAddresses(const std::string &hostNameOrAddress);
		//static IPHostEntry GetHostEntry(const IPAddress &address);
		//static IPHostEntry GetHostEntry(const std::string &hostNameOrAddress);
		static std::string GetHostName(const IPAddress &adddress);
	};
}