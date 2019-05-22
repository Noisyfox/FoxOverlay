#pragma once

#include "framework.h"

class WinMutex
{
private:
	HANDLE handle_;

public:
	WinMutex();
	explicit WinMutex(bool bInitialOwner);
	explicit WinMutex(LPCWSTR lpName);
	WinMutex(bool bInitialOwner, LPCWSTR lpName);

	~WinMutex() noexcept;

	WinMutex(const WinMutex&) = delete;
	WinMutex& operator=(const WinMutex&) = delete;
	WinMutex& operator=(WinMutex&&) = delete;
	WinMutex(WinMutex&&) = delete;

	void lock();

	void unlock();

	bool try_lock();
};
