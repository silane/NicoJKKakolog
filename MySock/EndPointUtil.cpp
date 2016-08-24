#include "EndPointUtil.h"
#include "EndPoint/RawEndPoint.h"
#include "EndPoint/UnixDomainEndPoint.h"
#include "EndPoint/IPEndPoint.h"

namespace MySock
{
	std::unique_ptr<EndPoint> EndPointUtil::FromSocketAddress(const SocketAddress &address)
	{
		EndPoint *ret = nullptr;
		switch (address.GetFamily())
		{
#ifndef  _WIN32
		case AF_UNIX:
			ret = new UnixDomainEndPoint("");
			break;
#endif // ! _WIN32
		case AF_INET:
		case AF_INET6:
			ret = new IPEndPoint(IPAddress::None,0);
			break;
		default:
			ret = new RawEndPoint();
			break;
		}
		ret->Create(address);
		return std::unique_ptr<EndPoint>(ret);
	}
}