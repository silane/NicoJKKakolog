#include "UnixDomainEndPoint.h"

#ifndef _WIN32

#include <sys/un.h>

namespace MySock
{
UnixDomainEndPoint::UnixDomainEndPoint(const std::string &path)
{
	if (path.size() + 1 > UNIX_PATH_MAX)
		throw std::invalid_argument("path size should be smaller than UNIX_PATH_MAX");
	this->path.assign(path);
}

sa_family_t UnixDomainEndPoint::GetAddressFamily() const noexcept
{
	return AF_UNIX;
}

void UnixDomainEndPoint::Create(const SocketAddress &rawAddress)
{
	if (rawAddress.GetFamily() != AF_UNIX)
		throw std::invalid_argument("address family should be AF_UNIX");

	sockaddr_un *addr = (sockaddr_un *)rawAddress.GetData();
	this->path.assign(addr->sun_path, UNIX_PATH_MAX);
}

SocketAddress UnixDomainEndPoint::Serialize() const
{
	SocketAddress rawaddr(AF_UNIX, sizeof(sockaddr_un));

	sockaddr_un *addr = (sockaddr_un *)rawaddr.GetData();
	addr->sun_path[this->path.copy(addr->sun_path, this->path.size())] = '\0';
}

EndPoint *UnixDomainEndPoint::Clone() const
{
	return new UnixDomainEndPoint(*this);
}

std::string UnixDomainEndPoint::GetPath() const
{
	return this->path;
}
}
#endif // !_WIN32