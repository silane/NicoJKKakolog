#include "Dns.h"
#include "CommonIncludes.h"
#include "SocketAddress.h"
#include "EndPointUtil.h"
#include "AddressInfoErrorCategory.h"

namespace MySock
{
	std::vector<IPAddress> Dns::GetHostAddresses(const std::string &hostNameOrAddress)
	{
		addrinfo hints = {0};
		hints.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG;
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = 0;
		hints.ai_protocol = 0;

		addrinfo *result;
		int code = getaddrinfo(hostNameOrAddress.c_str(), NULL, &hints, &result);
		if (code != 0)
		{
			throw std::system_error(code, AddressInfoErrorCategory());
		}

		std::vector<IPAddress> ret;
		for (addrinfo *rp = result; rp != NULL; rp = rp->ai_next)
		{
			if (rp->ai_family == AF_INET)
			{
				sockaddr_in *addr = (sockaddr_in *)rp->ai_addr;
				ret.emplace_back((unsigned char *)&addr->sin_addr, 4);
			}
			else if(rp->ai_family==AF_INET6)
			{
				sockaddr_in6 *addr = (sockaddr_in6 *)rp->ai_addr;
				ret.emplace_back((unsigned char *)&addr->sin6_addr, 16);
			}
		}
		freeaddrinfo(result);

		return ret;
	}

	std::string Dns::GetHostName(const IPAddress &address)
	{
		char host[NI_MAXHOST];
		if (address.GetAddressFamily() == AF_INET6)
		{
			sockaddr_in6 addr = { 0 };
			std::memcpy(&addr.sin6_addr, address.GetAddressBytes(), 16);
			addr.sin6_family = AF_INET6;

			if (int code = getnameinfo((sockaddr *)&addr, sizeof(addr), host, sizeof(host), NULL, 0, 0) != 0)
				throw std::system_error(code, AddressInfoErrorCategory());
		}
		else
		{
			sockaddr_in addr = { 0 };
			std::memcpy(&addr.sin_addr, address.GetAddressBytes(), 4);
			addr.sin_family = AF_INET;

			if (int code = getnameinfo((sockaddr *)&addr, sizeof(addr), host, sizeof(host), NULL, 0, 0) != 0)
				throw std::system_error(code, AddressInfoErrorCategory());
		}
		return host;
	}

	/*IPHostEntry Dns::GetHostEntry(const std::string &hostNameOrAddress)
	{
		addrinfo hints = { 0 };
		hints.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG;
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = 0;
		hints.ai_protocol = 0;

		addrinfo *result;
		int code = getaddrinfo(hostNameOrAddress.c_str(), NULL, &hints, &result);
		if (code != 0)
		{
			throw std::system_error(code, AddressInfoErrorCategory());
		}

		IPHostEntry ret;
		ret.HostName = hostNameOrAddress;
		for (addrinfo *rp = result; rp != NULL; rp = rp->ai_next)
		{
			if (rp->ai_family == AF_INET)
			{
				sockaddr_in *addr = (sockaddr_in *)rp->ai_addr;
				ret.AddressList.emplace_back((unsigned char *)&addr->sin_addr, 4);
			}
			else if (rp->ai_family == AF_INET6)
			{
				sockaddr_in6 *addr = (sockaddr_in6 *)rp->ai_addr;
				ret.AddressList.emplace_back((unsigned char *)&addr->sin6_addr, 16);
			}
		}
		freeaddrinfo(result);

		return ret;
	}*/
	//std::string Dns::GetHostName();
}