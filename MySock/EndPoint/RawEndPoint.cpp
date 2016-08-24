#include "RawEndPoint.h"
#include <cstring>

namespace MySock
{
	sa_family_t RawEndPoint::GetAddressFamily() const noexcept
	{
		if (this->data.empty())
			return AF_UNSPEC;
		sockaddr *addr = (sockaddr *)this->data.data();
		return addr->sa_family;
	}

	void RawEndPoint::Create(const SocketAddress &rawAddress)
	{
		this->data.resize(rawAddress.GetSize());
		std::memcpy(this->data.data(), rawAddress.GetData(), rawAddress.GetSize());
	}

	SocketAddress RawEndPoint::Serialize() const
	{
		SocketAddress addr(this->data.size());
		std::memcpy(addr.GetData(), this->data.data(), this->data.size());
		return addr;
	}

	EndPoint *RawEndPoint::Clone() const
	{
		return new RawEndPoint(*this);
	}

	std::vector<unsigned char> &RawEndPoint::GetData() noexcept
	{
		return this->data;
	}
}