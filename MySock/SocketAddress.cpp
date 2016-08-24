#include "SocketAddress.h"
#include <cstring>
#include <stdexcept>

namespace MySock
{
	SocketAddress::SocketAddress(std::size_t size) :
		size(size), hasOwnership(true)
	{
		if (size < sizeof(sockaddr))
			throw std::out_of_range("size can't be smaller than sizeof(sockaddr)");

		this->data=new char[size];
	}

	SocketAddress::SocketAddress(sa_family_t family, std::size_t size) :
		size(size), hasOwnership(true)
	{
		if (size < sizeof(sockaddr))
			throw std::out_of_range("size can't be smaller than sizeof(sockaddr)");

		this->data=new char[size];
		sockaddr *addr = (sockaddr *)this->data;
		addr->sa_family = family;
	}

	SocketAddress::SocketAddress(void *data, std::size_t size)
		:data((char *)data),size(size),hasOwnership(false)
	{
	}

	SocketAddress::SocketAddress(const SocketAddress &val) :
		data(new char[val.size]), size(val.size), hasOwnership(true)
	{
		std::memcpy(this->data, val.data, val.size);
	}

	SocketAddress::SocketAddress(SocketAddress &&val) :
		data(val.data), size(val.size), hasOwnership(val.hasOwnership)
	{
		val.data = nullptr;
	}

	SocketAddress &SocketAddress::operator=(const SocketAddress &val)
	{
		this->data=new char[val.size];
		this->size = val.size;
		this->hasOwnership = true;

		std::memcpy(this->data, val.data, val.size);

		return *this;
	}

	SocketAddress &SocketAddress::operator=(SocketAddress &&val)
	{
		this->data = val.data;
		this->size = val.size;
		this->hasOwnership = val.hasOwnership;

		val.data = nullptr;

		return *this;
	}

	SocketAddress::~SocketAddress()
	{
		if(hasOwnership)
			delete [] this->data;
	}

	sa_family_t SocketAddress::GetFamily() const noexcept
	{
		sockaddr *addr = (sockaddr *)this->data;
		return addr->sa_family;
	}

	void *SocketAddress::GetData() const noexcept
	{
		return this->data;
	}

	std::size_t SocketAddress::GetSize() const noexcept
	{
		return this->size;
	}

	void SocketAddress::Shrink(std::size_t size)
	{
		if (size > this->size)
			throw std::out_of_range("size can't be larger than current size");
		if(size<sizeof(sockaddr))
			throw std::out_of_range("size can't be smaller than sizeof(sockaddr)");

		this->size = size;
	}
}