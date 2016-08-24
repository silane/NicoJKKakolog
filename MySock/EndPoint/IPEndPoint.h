#pragma once

#include "EndPoint.h"
#include "../IPAddress.h"

namespace MySock
{
	class IPEndPoint :public EndPoint
	{
	private:
		IPAddress address;
		sa_family_t addressFamily;
		in_port_t port;

	public:
		virtual sa_family_t GetAddressFamily() const noexcept override;
		virtual void Create(const SocketAddress &rawAddress) override;
		virtual SocketAddress Serialize() const override;
		virtual EndPoint *Clone() const override;

		IPEndPoint(const IPAddress &address, in_port_t port);
		IPEndPoint(const IPEndPoint &val) = default;
		IPEndPoint(IPEndPoint &&val) = default;
		IPEndPoint &operator=(const IPEndPoint &val) = default;
		IPEndPoint &operator=(IPEndPoint &&val) = default;

		IPAddress GetAddress() const;
		in_port_t GetPort() const;
	};
}