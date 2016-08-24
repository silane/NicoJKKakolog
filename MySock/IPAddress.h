#pragma once

#include "CommonIncludes.h"
#include <stdint.h>
#include <string>

namespace MySock
{
	class IPAddress
	{
	private:
		unsigned char bytes[16];
		int size;

	public:
		static const IPAddress Any;
		static const IPAddress Broadcast;
		static const IPAddress IPv6Any;
		static const IPAddress IPv6Loopback;
		static const IPAddress IPv6None;
		static const IPAddress Loopback;
		static const IPAddress None;

	public:
		static uint16_t HostToNetworkOrder(uint16_t host);
		static uint32_t HostToNetworkOrder(uint32_t host);
		static uint64_t HostToNetworkOrder(uint64_t host);
		static uint16_t NetworkToHostOrder(uint16_t network);
		static uint32_t NetworkToHostOrder(uint32_t network);
		static uint64_t NetworkToHostOrder(uint64_t network);

		IPAddress(const unsigned char address[], int size);//address is regarded as network byte order
		explicit IPAddress(uint32_t v4address);//prameter is regarded as host byte order
		IPAddress(uint64_t v6address_hi, uint64_t v6address_lo);//prameters are regarded as host byte order

		sa_family_t GetAddressFamily() const;
		const unsigned char *GetAddressBytes() const;
		int GetAddressSize() const;

		bool IsLoopback() const;

		static IPAddress Parse(const std::string &ipString, sa_family_t family = AF_UNSPEC);
		std::string ToString() const;


		IPAddress(const IPAddress &address) = default;
		IPAddress(IPAddress &&address) = default;
		IPAddress &operator=(const IPAddress &address) = default;
		IPAddress &operator=(IPAddress &&address) = default;

		bool operator==(const IPAddress &address) const;
		bool operator!=(const IPAddress &address) const;

	};
}