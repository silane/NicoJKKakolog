#pragma once
#include "EndPoint.h"
#include <vector>

namespace MySock
{
	class RawEndPoint :public EndPoint
	{
	private:
		std::vector<unsigned char> data;

	public:
		virtual sa_family_t GetAddressFamily() const noexcept override;
		virtual void Create(const SocketAddress &rawAddress) override;
		virtual SocketAddress Serialize() const override;
		virtual EndPoint *Clone() const override;

		RawEndPoint() = default;
		RawEndPoint(const RawEndPoint &val) = default;
		RawEndPoint(RawEndPoint &&val) = default;
		RawEndPoint &operator=(const RawEndPoint &val) = default;
		RawEndPoint &operator=(RawEndPoint &&val) = default;

		std::vector<unsigned char> &GetData() noexcept;
	};
}