#pragma once

#include "framework.h"
#include "MinHook.h"

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


template <
	typename ReturnType,
	typename... ArgumentType
>
class ApiHook
{
public:
	using TargetFuncType = ReturnType(WINAPI*)(ArgumentType ...);

private:
	TargetFuncType pHookedOriginalFunc_{nullptr};
	TargetFuncType pTargetFunc_{nullptr};
	TargetFuncType pHookFunc_{nullptr};

public:

	ApiHook(TargetFuncType pTargetFunc, TargetFuncType pHookFunc):
		pTargetFunc_(pTargetFunc), pHookFunc_(pHookFunc)
	{
	}

	~ApiHook()
	{
		unhook();
	}

	bool hook()
	{
		if (!hooked())
		{
			auto s = MH_CreateHook(reinterpret_cast<LPVOID>(pTargetFunc_), reinterpret_cast<LPVOID>(pHookFunc_),
			                       reinterpret_cast<LPVOID*>(&pHookedOriginalFunc_));
			if (s == MH_OK)
			{
				s = MH_EnableHook(reinterpret_cast<LPVOID>(pTargetFunc_));
				if (s == MH_OK)
				{
					_ASSERT(pHookedOriginalFunc_ != nullptr);
				}
				else
				{
					OutputDebugString(_T("MH_EnableHook failed!"));
					MH_RemoveHook(reinterpret_cast<LPVOID>(pTargetFunc_));
					pHookedOriginalFunc_ = nullptr;
				}
			}
			else
			{
				OutputDebugString(_T("MH_CreateHook failed!"));
				pHookedOriginalFunc_ = nullptr;
			}
		}

		return hooked();
	}

	void unhook()
	{
		if (hooked())
		{
			MH_RemoveHook(reinterpret_cast<LPVOID>(pTargetFunc_));
			pHookedOriginalFunc_ = nullptr;
		}
	}

	ReturnType callOrigin(ArgumentType ... args)
	{
		return pHookedOriginalFunc_(std::forward<ArgumentType>(args)...);
	}

	bool hooked() const
	{
		return pHookedOriginalFunc_ != nullptr;
	}
};
