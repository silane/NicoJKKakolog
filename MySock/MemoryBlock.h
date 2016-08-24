#pragma once

#include <string>
#include <vector>
#include <cstring>

class MemoryBlock
{
private:
	unsigned char *buf;
	size_t len;
	size_t capacity;
	
public:
	MemoryBlock(const MemoryBlock &obj) = delete;
	MemoryBlock &operator=(const MemoryBlock &obj) = delete;

	template<typename charT>
	static MemoryBlock FromString(const std::basic_string<charT> &str) 
	{
		MemoryBlock ret(str.size()*sizeof(charT));
		std::memcpy(ret.buf, str.data(), str.size() * sizeof(charT));
		return ret;
	}
	static MemoryBlock FromVector(const std::vector<unsigned char> &vec);

	MemoryBlock(MemoryBlock &&obj) noexcept;
	MemoryBlock &operator=(MemoryBlock &&obj) noexcept;

	MemoryBlock(size_t size);
	void *GetBuf() const noexcept;
	size_t GetSize() const noexcept;
	size_t GetCapacity() const noexcept;
	void Resize(size_t size);

	template<typename charT>
	std::basic_string<charT> ToString() const
	{
		std::basic_string<charT> ret((charT *)this->buf, this->len / sizeof(charT));
		return ret;
	}
	std::vector<unsigned char> ToVector() const;
	
	~MemoryBlock();
};

