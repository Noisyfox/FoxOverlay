#include "Overlay.h"
#include "Session.h"

Overlay::Overlay():
	dwProcId_(GetCurrentProcessId())
{
}

Overlay& Overlay::instance()
{
	static Overlay overlay;

	return overlay;
}

bool Overlay::setModuleHandle(const HMODULE hModule)
{
	std::lock_guard<std::mutex> lock(overlayStateLock_);

	moduleHandle_ = hModule;

	return true;
}

HMODULE Overlay::getModuleHandle() const
{
	return moduleHandle_;
}

void Overlay::startup()
{
	std::lock_guard<std::mutex> lock(overlayStateLock_);

	if (currentSession_ == nullptr || !currentSession_->isAlive())
	{
		// Start new hook session
		currentSession_.reset(new Session(this));
	}
}

bool Overlay::shutdown()
{
	std::shared_ptr<Session> currentSession;
	{
		std::lock_guard<std::mutex> lock(overlayStateLock_);
		currentSession = currentSession_;
		currentSession_.reset();
	}

	if (currentSession != nullptr)
	{
		currentSession->shutdownAndWait();

		return true;
	}

	return false;
}
