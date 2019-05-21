#pragma once

#include "framework.h"
#include <mutex>
#include "Session.h"

class Overlay
{
private:
	std::mutex overlayStateLock_;

	const DWORD dwProcId_;
	HMODULE moduleHandle_ = nullptr;

	friend class Session;
	std::shared_ptr<Session> currentSession_;

	Overlay();

public:
	static Overlay& instance();

	bool setModuleHandle(HMODULE);
	HMODULE getModuleHandle() const;

	void startup();
	bool shutdown();
};
