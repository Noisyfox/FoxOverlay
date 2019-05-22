#include "WinMutex.h"
#include <stdexcept>

WinMutex::WinMutex():
	WinMutex(false, nullptr)
{
}

WinMutex::WinMutex(bool bInitialOwner) :
	WinMutex(bInitialOwner, nullptr)
{
}

WinMutex::WinMutex(LPCWSTR lpName) :
	WinMutex(false, lpName)
{
}

WinMutex::WinMutex(bool bInitialOwner, LPCWSTR lpName)
{
	auto hMutex = CreateMutex(nullptr, bInitialOwner, lpName);

	if (hMutex == nullptr)
	{
		throw std::invalid_argument("handle can not be nullptr.");
	}

	handle_ = hMutex;
}

WinMutex::~WinMutex() noexcept
{
	CloseHandle(handle_);
}

void WinMutex::lock()
{
	const auto dwWaitResult = WaitForSingleObject(handle_, INFINITE);

	switch (dwWaitResult)
	{
	case WAIT_OBJECT_0:
		// Success
		break;
	case WAIT_ABANDONED:
		OutputDebugString(_T("WAIT_ABANDONED"));
		break;
	case WAIT_FAILED:
		throw std::runtime_error("Unable to acquire mutex: WAIT_FAILED");
	default:
		throw std::runtime_error("Unable to acquire mutex: UNKNOWN");
	}
}

void WinMutex::unlock()
{
	if (!ReleaseMutex(handle_))
	{
		// TODO: more specific error
		// TODO: check GetLastError()
		throw std::runtime_error("Unable to release mutex!");
	}
}

bool WinMutex::try_lock()
{
	const auto dwWaitResult = WaitForSingleObject(handle_, 0);

	switch (dwWaitResult)
	{
	case WAIT_OBJECT_0:
		// Success
		return true;
	case WAIT_ABANDONED:
		OutputDebugString(_T("WAIT_ABANDONED"));
		return true;
	case WAIT_TIMEOUT:
		return false;
	case WAIT_FAILED:
		throw std::runtime_error("Unable to acquire mutex: WAIT_FAILED");
	default:
		throw std::runtime_error("Unable to acquire mutex: UNKNOWN");
	}
}
