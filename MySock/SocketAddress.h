#pragma once
#include "CommonIncludes.h"
#include <memory>

namespace MySock
{
	class SocketAddress
	{
	private:
		char *data;
		std::size_t size;
		bool hasOwnership;

	public:
		SocketAddress(std::size_t size);
		SocketAddress(sa_family_t family, std::size_t size);
		SocketAddress(void *data, std::size_t size);//assign pointer, data will not be deleted at destructor

		SocketAddress(const SocketAddress &val);
		SocketAddress(SocketAddress &&val);
		SocketAddress &operator=(const SocketAddress &val);
		SocketAddress &operator=(SocketAddress &&val);
		~SocketAddress();

		sa_family_t GetFamily() const noexcept;
		void *GetData() const noexcept;
		std::size_t GetSize() const noexcept;

		void Shrink(std::size_t size);
	};
}