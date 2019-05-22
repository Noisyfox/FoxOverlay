#pragma once

#include "framework.h"

enum class Endianness
{
	BigEndian,
	LittleEndian
};

template <typename T>
T SwapBytes(const T v)
{
	T r = v;
	auto b = reinterpret_cast<BYTE*>(&r);
	std::reverse(b, b + sizeof(T));

	return r;
}

static Endianness GetNativeEndianess()
{
	UINT16 v = 1;
	const auto b = reinterpret_cast<BYTE*>(&v);

	if (b[0])
	{
		return Endianness::LittleEndian;
	}
	else
	{
		return Endianness::BigEndian;
	}
}

template <typename T>
T FromLittleEndian(const T v)
{
	return GetNativeEndianess() == Endianness::LittleEndian
		       ? v
		       : SwapBytes(v);
}

template <typename T>
T FromBigEndian(const T v)
{
	return GetNativeEndianess() == Endianness::BigEndian
		       ? v
		       : SwapBytes(v);
}
