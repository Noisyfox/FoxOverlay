#pragma once

#include "framework.h"
#include "Endianness.h"

class MemoryBinaryReader
{
private:
	const LPCVOID pBuffer_;
	size_t dwBufferSize_;
	size_t dwPos_ = 0;

public:
	MemoryBinaryReader(LPCVOID pBuffer, size_t dwBufferSize);

	size_t size() const;

	size_t pos() const;

	void resize(size_t dwNewSize);

	void skip(size_t dwCount);

	void seek(size_t dwPos);

	void read(LPVOID pOut, size_t dwSize);

	template <typename T>
	T read()
	{
		static_assert(
			sizeof(T) == 1,
			"Endianness must be specified for multi-byte types");

		T v;
		read(&v, sizeof(v));

		return v;
	}

	template <typename T>
	T read_le()
	{
		static_assert(
			sizeof(T) > 1,
			"Endianness does not make sense for single-byte types");

		T v;
		read(&v, sizeof(v));

		return FromLittleEndian(v);
	}

	template <typename T>
	T read_be()
	{
		static_assert(
			sizeof(T) > 1,
			"Endianness does not make sense for single-byte types");

		T v;
		read(&v, sizeof(v));

		return FromBigEndian(v);
	}
};
