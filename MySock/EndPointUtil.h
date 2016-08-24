#pragma once
#include "EndPoint/EndPoint.h"
#include <memory>

namespace MySock
{
	class EndPointUtil
	{
	public:
		static std::unique_ptr<EndPoint> FromSocketAddress(const SocketAddress &address);
	};
}