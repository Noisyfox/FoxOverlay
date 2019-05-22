#include "MemoryBinaryReader.h"
#include <stdexcept>

MemoryBinaryReader::MemoryBinaryReader(const LPCVOID pBuffer, const size_t dwBufferSize) :
	pBuffer_(pBuffer), dwBufferSize_(dwBufferSize)
{
}

size_t MemoryBinaryReader::size() const
{
	return dwBufferSize_;
}

size_t MemoryBinaryReader::pos() const
{
	return dwPos_;
}

void MemoryBinaryReader::resize(const size_t dwNewSize)
{
	dwBufferSize_ = dwNewSize;
	if (dwPos_ > dwNewSize)
	{
		dwPos_ = dwNewSize;
	}
}

void MemoryBinaryReader::skip(const size_t dwCount)
{
	const auto dwNewPos = dwPos_ + dwCount;
	if (dwNewPos > dwBufferSize_)
	{
		throw std::out_of_range("EOF");
	}

	dwPos_ = dwNewPos;
}

void MemoryBinaryReader::seek(const size_t dwPos)
{
	if (dwPos > dwBufferSize_)
	{
		throw std::out_of_range("EOF");
	}

	dwPos_ = dwPos;
}

void MemoryBinaryReader::read(const LPVOID pOut, const size_t dwSize)
{
	if (dwPos_ + dwSize > size())
	{
		throw std::out_of_range("EOF");
	}

	const auto pSrc = reinterpret_cast<const BYTE*>(pBuffer_) + dwPos_;
	dwPos_ += dwSize;

	std::memcpy(pOut, pSrc, dwSize);
}
