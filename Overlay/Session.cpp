#include "Session.h"
#include "Overlay.h"
#include "scope_guard.hpp"
#include "EventLoop.h"


DWORD WINAPI SessionThread(_In_ LPVOID lpParam)
{
	auto session = static_cast<Session*>(lpParam);
	return session->runThread();
}

Session::Session(Overlay* owner):
	owner_(owner),
	moduleHandle_(owner->getModuleHandle()),
	ev_(new EventLoop)
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

	// TODO: connect to service

	// TODO: see if service ask us to hook a given window

	// Otherwise try finding a window
	// if (!findGameWindow())
	// {
	// 	EXIT_THREAD(2);
	// }

	// Hook the window

	if (ev_->prepareExitHandle())
	{
		EXIT_THREAD(0);
	}

	// Test: close after 10s
	ev_->loop()->update();
	auto t = ev_->loop()->resource<uvw::TimerHandle>();
	t->on<uvw::TimerEvent>([this](const uvw::TimerEvent&, const uvw::TimerHandle&)
	{
		shutdownAndWait();
	});
	t->start(uvw::TimerHandle::Time{10000}, uvw::TimerHandle::Time{0});

	ev_->run();

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
	// Ask thread to exit
	ev_->stopGracefully();

	// Make sure it's not called from the session thread, otherwise an infinity loop
	// will occur since it's waiting itself to exit.
	if (GetThreadId(sessionThread_) != GetCurrentThreadId())
	{
		while (isAlive())
		{
			Sleep(100);
		}
	}

	// TODO: free all resources
}

struct EnumWindowsParam
{
	DWORD dwProcessId;
	HWND gameWindow;
};

BOOL CALLBACK EnumWindowsFunc(const HWND hwnd, const LPARAM lParam)
{
	const auto param = reinterpret_cast<EnumWindowsParam*>(lParam);

	// Skip any window from other processes
	DWORD currentProcessId = 0;
	GetWindowThreadProcessId(hwnd, &currentProcessId);
	if (param->dwProcessId != currentProcessId)
	{
		return TRUE;
	}

	// Skip any child window
	const auto dwStyle = static_cast<DWORD>(GetWindowLongPtr(hwnd, GWL_STYLE));
	if (dwStyle & WS_CHILDWINDOW)
	{
		return TRUE;
	}

	// Skip any hidden window
	if (!IsWindowVisible(hwnd))
	{
		return TRUE;
	}

	// TODO: skip any window that created by this plugin, for example, debugging console window

	param->gameWindow = hwnd;
	return FALSE;
}

bool Session::findGameWindow()
{
	if (gameWindow_)
	{
		return true;
	}

	EnumWindowsParam sParam = {GetCurrentProcessId(), nullptr};
	EnumWindows(EnumWindowsFunc, reinterpret_cast<LPARAM>(&sParam));

	if (!sParam.gameWindow)
	{
		OutputDebugString(_T("Unable to find any window\n"));
		return false;
	}

	gameWindow_ = sParam.gameWindow;
	return true;
}
