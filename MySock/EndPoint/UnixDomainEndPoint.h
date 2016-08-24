#pragma once

#include "EndPoint.h"
#ifndef _WIN32
namespace MySock
{
class UnixDomainEndPoint:public EndPoint
{
private:
	std::string path;

public:
	virtual sa_family_t GetAddressFamily() const noexcept override;
	virtual void Create(const SocketAddress &rawAddress) override;
	virtual SocketAddress Serialize() const override;
	virtual EndPoint *Clone() const override;

	UnixDomainEndPoint(const std::string &path);
	UnixDomainEndPoint(const UnixDomainEndPoint &val) = default;
	UnixDomainEndPoint(UnixDomainEndPoint &&val) = default;
	UnixDomainEndPoint &operator=(const UnixDomainEndPoint &val) = default;
	UnixDomainEndPoint &operator=(UnixDomainEndPoint &&val) = default;
	
	std::string GetPath() const;
};
}
#endif // !_WIN32
