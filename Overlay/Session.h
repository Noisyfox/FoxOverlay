#pragma once

#include "framework.h"
#include "GlobalConfiguration.h"
#include <mutex>

class Overlay;

class Session
{
private:
	std::mutex stateLock_;

	friend class Overlay;
	Overlay* const owner_;
	const HMODULE moduleHandle_;

	std::shared_ptr<GlobalConfiguration> configuration_;

	HANDLE sessionThread_ = nullptr;

	Session(Overlay* owner);

	friend DWORD WINAPI SessionThread(_In_ LPVOID);
	DWORD runThread();

	bool isAlive() const;

	void shutdownAndWait();
public:
	~Session();
};
