#include "MemoryBlock.h"

MemoryBlock MemoryBlock::FromVector(const std::vector<unsigned char> &vec)
{
	MemoryBlock ret(vec.size());
	std::memcpy(ret.buf, vec.data(), ret.len);
	return ret;
}


MemoryBlock::MemoryBlock(MemoryBlock &&obj) noexcept:
	buf(obj.buf),len(obj.len),capacity(obj.capacity)
{
	obj.buf = nullptr;
	obj.len = 0;
	obj.capacity = 0;
}

MemoryBlock &MemoryBlock::operator=(MemoryBlock &&obj) noexcept
{
	this->buf = obj.buf;
	this->len = obj.len;
	this->capacity = obj.capacity;
	obj.buf = nullptr;
	obj.len = 0;
	obj.capacity = 0;

	return *this;
}

MemoryBlock::MemoryBlock(size_t size):
	buf(new unsigned char[size]),len(size),capacity(size)
{
}

void *MemoryBlock::GetBuf() const noexcept
{
	return this->buf;
}

size_t MemoryBlock::GetSize() const noexcept
{
	return this->len;
}

size_t MemoryBlock::GetCapacity() const noexcept
{
	return this->capacity;
}

void MemoryBlock::Resize(size_t size)
{
	if (size > this->capacity)
	{
		delete[] this->buf;
		this->buf = new unsigned char[size];
		this->len = size;
		this->capacity = size;
	}
	else
	{
		this->len = size;
	}
}

std::vector<unsigned char> MemoryBlock::ToVector() const
{
	std::vector<unsigned char> ret(this->buf,this->buf+this->len);
	return ret;
}

MemoryBlock::~MemoryBlock()
{
	delete[] buf;
}
