#pragma once

#include "framework.h"

inline PVOID getVFunction(const PVOID obj, const int index)
{
	if (!obj)
	{
		return nullptr;
	}

	const auto pVTable = reinterpret_cast<PVOID*>(* reinterpret_cast<PVOID*>(obj));
	if (!pVTable)
	{
		return nullptr;
	}

	return pVTable[index];
}
