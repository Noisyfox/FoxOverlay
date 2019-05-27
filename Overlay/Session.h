#pragma once

#include "framework.h"
#include "GlobalConfiguration.h"
#include <mutex>

class Overlay;
class EventLoop;

class Session
{
private:
	std::mutex stateLock_;

	friend class Overlay;
	Overlay* const owner_;
	const HMODULE moduleHandle_;

	std::shared_ptr<GlobalConfiguration> configuration_;
	HANDLE sessionThread_ = nullptr;
	std::shared_ptr<EventLoop> ev_;

	HWND gameWindow_ = nullptr;

	explicit Session(Overlay* owner);

	friend DWORD WINAPI SessionThread(_In_ LPVOID);
	DWORD runThread();

	bool isAlive() const;

	void shutdownAndWait();

	bool findGameWindow();
public:
	~Session();
};
