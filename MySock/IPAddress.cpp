#include "IPAddress.h"
#include <cstring>
#include <stdexcept>

namespace MySock
{
	const IPAddress IPAddress::Any(0);
	const IPAddress IPAddress::Broadcast(0xffffffff);
	const IPAddress IPAddress::IPv6Any(0LLU, 0LLU);
	const IPAddress IPAddress::IPv6Loopback(0LLU, 1LLU);
	const IPAddress IPAddress::IPv6None(0LLU, 0LLU);
	const IPAddress IPAddress::Loopback(0x7f000001);
	const IPAddress IPAddress::None(0xffffffff);

	uint16_t IPAddress::HostToNetworkOrder(uint16_t host)
	{
		return htons(host);
	}

	uint32_t IPAddress::HostToNetworkOrder(uint32_t host)
	{
		return htonl(host);
	}

	uint64_t IPAddress::HostToNetworkOrder(uint64_t host)
	{
		// The answer is 42
		constexpr int num = 42;

		// Check the endianness
		if (*reinterpret_cast<const char*>(&num) == num)
		{
			const uint32_t high_part = htonl(static_cast<uint32_t>(host >> 32));
			const uint32_t low_part = htonl(static_cast<uint32_t>(host & 0xFFFFFFFFLL));

			return (static_cast<uint64_t>(low_part) << 32) | high_part;
		}
		else
		{
			return host;
		}
	}

	uint16_t IPAddress::NetworkToHostOrder(uint16_t network)
	{
		return ntohs(network);
	}

	uint32_t IPAddress::NetworkToHostOrder(uint32_t network)
	{
		return ntohl(network);
	}

	uint64_t IPAddress::NetworkToHostOrder(uint64_t network)
	{
		return HostToNetworkOrder(network);
	}

	IPAddress::IPAddress(const unsigned char address[], int size) :
		size(size)
	{
		if (size != 4 && size != 16)
			throw std::invalid_argument("size should be 4 or 16");
		std::memcpy(this->bytes, address, (std::size_t)size);
	}

	IPAddress::IPAddress(uint32_t v4address) :
		size(4)
	{
		uint32_t addr = HostToNetworkOrder(v4address);

		std::memcpy(this->bytes, &addr, 4);
	}

	IPAddress::IPAddress(uint64_t v6address_hi, uint64_t v6address_lo)
	{
		v6address_hi = HostToNetworkOrder(v6address_hi);
		v6address_lo = HostToNetworkOrder(v6address_lo);
		std::memcpy(this->bytes + 8, &v6address_hi, 8);
		std::memcpy(this->bytes, &v6address_lo, 8);
	}

	sa_family_t IPAddress::GetAddressFamily() const
	{
		return (this->size == 4) ? AF_INET : AF_INET6;
	}

	const unsigned char *IPAddress::GetAddressBytes() const
	{
		return this->bytes;
	}

	int IPAddress::GetAddressSize() const
	{
		return this->size;
	}

	bool IPAddress::IsLoopback() const
	{
		if (this->size == 4)
			return this->bytes[3] == 127;
		else
			return *this == IPAddress::Loopback;
	}

	IPAddress IPAddress::Parse(const std::string &ipString, sa_family_t family)
	{
		IPAddress ret(0U);

		if (family == AF_UNSPEC)
		{
			if (ipString.find('.') != std::string::npos)
				family = AF_INET;
			else if (ipString.find(':') != std::string::npos)
				family = AF_INET6;
		}

		if (family == AF_INET)
		{
			if (inet_pton(AF_INET, ipString.c_str(), ret.bytes) != 1)
				throw std::invalid_argument("failed to parse as IPv4");
		}
		else if (family == AF_INET6)
		{
			if (inet_pton(AF_INET6, ipString.c_str(), ret.bytes) != 1)
				throw std::invalid_argument("failed to parse as IPv6");
		}
		else if (family == AF_UNSPEC)
			throw std::invalid_argument("failed to parse as IP address");
		else
			throw std::invalid_argument("family should be AF_INET, AF_INET6 or AF_UNSPEC");

		return ret;
	}

	std::string IPAddress::ToString() const
	{
		if (this->size == 4)
		{
			char buf[INET_ADDRSTRLEN];
			if (inet_ntop(AF_INET, (void *)this->bytes, buf, sizeof(buf)) == NULL)
				throw std::logic_error("failed to convert to string");
			return buf;
		}
		else
		{
			char buf[INET6_ADDRSTRLEN];
			if (inet_ntop(AF_INET6, (void *)this->bytes, buf, sizeof(buf)) == NULL)
				throw std::logic_error("failed to convert to string");
			return buf;
		}
	}

	bool IPAddress::operator==(const IPAddress &address) const
	{
		return address.size == this->size && std::memcmp(address.bytes, this->bytes, this->size) == 0;
	}

	bool IPAddress::operator!=(const IPAddress &address) const
	{
		return !(*this == address);
	}
}