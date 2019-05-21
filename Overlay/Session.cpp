#include "Session.h"
#include "Overlay.h"
#include "scope_guard.hpp"

DWORD WINAPI SessionThread(_In_ LPVOID lpParam)
{
	auto session = static_cast<Session*>(lpParam);
	return session->runThread();
}

Session::Session(Overlay* owner):
	owner_(owner),
	moduleHandle_(owner->getModuleHandle())
{
	std::lock_guard<std::mutex> lock(stateLock_);

#ifdef _DEBUG
	// TODO: start debug console
#endif

	// Load the dll again so it won't be detached when injector exited
	TCHAR lpFilename[MAX_PATH];
	GetModuleFileName(moduleHandle_, lpFilename, MAX_PATH);
	LoadLibrary(lpFilename);

	// Start session thread
	sessionThread_ = CreateThread(nullptr, 0, SessionThread, this, 0, nullptr);
}

Session::~Session()
{
	shutdownAndWait();

	// Free thread handle
	CloseHandle(sessionThread_);
}


#define EXIT_THREAD(exit_code) {dwExitCode = (exit_code); return dwExitCode;} while(0)

DWORD Session::runThread()
{
	DWORD dwExitCode = 0;

	auto cleanupGuard = sg::make_scope_guard([this, &dwExitCode]
	{
		// TODO: free all resources

		// Free library when Overlay main thread exits
		FreeLibraryAndExitThread(moduleHandle_, dwExitCode);
	});

#ifdef _DEBUG
	MessageBox(nullptr, _T("Session::runThread"), _T(""), MB_OK);
#endif

	// Load configuration from a global shared memory
	configuration_ = GlobalConfiguration::load();
	if (configuration_ == nullptr)
	{
		OutputDebugString(_T("Unable to load global configuration, exiting...\n"));

		EXIT_THREAD(1);
	}

	EXIT_THREAD(0);
}
#undef EXIT_THREAD

bool Session::isAlive() const
{
	const auto dwResult = WaitForSingleObject(sessionThread_, 0);
	if (dwResult == WAIT_TIMEOUT)
	{
		return true;
	}

	DWORD dwExitCode = 0;
	if (GetExitCodeThread(sessionThread_, &dwExitCode))
	{
		return dwExitCode == STILL_ACTIVE;
	}

	return false;
}

void Session::shutdownAndWait()
{
	// Make sure it's not called from the session thread, otherwise an infinity loop
	// will occur since it's waiting itself to exit.
	if (GetThreadId(sessionThread_) != GetCurrentThreadId())
	{
		// TODO: ask thread to exit

		while (isAlive())
		{
			Sleep(100);
		}
	}

	// TODO: free all resources
}
