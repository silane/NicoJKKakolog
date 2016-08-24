#pragma once
#include "../SocketAddress.h"

namespace MySock
{
	class EndPoint
	{
	public:
		virtual sa_family_t GetAddressFamily() const noexcept = 0;
		virtual void Create(const SocketAddress &rawAddress) = 0;
		virtual SocketAddress Serialize() const = 0;

		virtual EndPoint *Clone() const = 0;

		virtual ~EndPoint() = default;
	};
}