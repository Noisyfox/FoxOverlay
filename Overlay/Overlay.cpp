#include "Overlay.h"
#include "Session.h"

static std::shared_ptr<Overlay> g_instance(nullptr);

Overlay::Overlay():
	dwProcId_(GetCurrentProcessId())
{
}

void Overlay::prepareInstance()
{
	g_instance.reset(new Overlay());
}

std::shared_ptr<Overlay> Overlay::instance()
{
	return g_instance;
}

void Overlay::freeInstance()
{
	g_instance.reset();
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
