#pragma once

#include "framework.h"
#include <mutex>

class Session;

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
	static void prepareInstance();
	static std::shared_ptr<Overlay> instance();
	static void freeInstance();

	bool setModuleHandle(HMODULE);
	HMODULE getModuleHandle() const;

	void startup();
	bool shutdown();
};
