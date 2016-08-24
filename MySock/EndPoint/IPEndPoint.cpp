#include "IPEndPoint.h"


namespace MySock
{
	sa_family_t IPEndPoint::GetAddressFamily() const noexcept
	{
		return this->address.GetAddressFamily();
	}

	void IPEndPoint::Create(const SocketAddress &rawAddress)
	{
		switch (rawAddress.GetFamily())
		{
		case AF_INET:
		{
			sockaddr_in *addr = (sockaddr_in *)rawAddress.GetData();
			this->address = IPAddress((unsigned char *)&addr->sin_addr, 4);
			this->port = IPAddress::NetworkToHostOrder(addr->sin_port);
			this->addressFamily = AF_INET;
			break;
		}
		case AF_INET6:
		{
			sockaddr_in6 *addr = (sockaddr_in6 *)rawAddress.GetData();
			this->address = IPAddress((unsigned char *)&addr->sin6_addr, 16);
			this->port = IPAddress::NetworkToHostOrder(addr->sin6_port);
			this->addressFamily = AF_INET6;
		}
		default:
			throw std::invalid_argument("address family should be AF_INET or AF_INET6");
		}
	}

	SocketAddress IPEndPoint::Serialize() const
	{
		if (address.GetAddressFamily() == AF_INET6)
		{
			SocketAddress rawAddr(sizeof(sockaddr_in6));
			sockaddr_in6 *addr = (sockaddr_in6 *)rawAddr.GetData();
			std::memcpy(&addr->sin6_addr, this->address.GetAddressBytes(), 16);
			addr->sin6_port = IPAddress::HostToNetworkOrder(this->port);
			addr->sin6_family = AF_INET6;

			return rawAddr;
		}
		else
		{
			SocketAddress rawAddr(sizeof(sockaddr_in));
			sockaddr_in *addr = (sockaddr_in *)rawAddr.GetData();
			std::memcpy(&addr->sin_addr, this->address.GetAddressBytes(), 4);
			addr->sin_port = IPAddress::HostToNetworkOrder(this->port);
			addr->sin_family = AF_INET;

			return rawAddr;
		}
	}

	EndPoint *IPEndPoint::Clone() const
	{
		return new IPEndPoint(*this);
	}

	IPEndPoint::IPEndPoint(const IPAddress &address, in_port_t port) :
		address(address), port(port)
	{
	}

	IPAddress IPEndPoint::GetAddress() const
	{
		return this->address;
	}

	in_port_t IPEndPoint::GetPort() const
	{
		return this->port;
	}
}