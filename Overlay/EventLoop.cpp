#include "framework.h"
#include "EventLoop.h"

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "userenv.lib")
#pragma comment(lib, "ws2_32.lib")

EventLoop::EventLoop()
{
	// Init loop
	loop_ = uvw::Loop::create();
	if (!loop_)
	{
		throw std::runtime_error("Unable to create event loop!");
	}
	loop_->on<uvw::ErrorEvent>([](const uvw::ErrorEvent& ev, const uvw::Loop&)
	{
		OutputDebugString(_T("Loop ErrorEvent: "));
		OutputDebugStringA(ev.name());
		OutputDebugString(_T("\n"));
		if (ev.code() == UV_EBUSY)
		{
			// Something is leaking!
			OutputDebugString(_T("uv_loop_close = UV_EBUSY, resource leaking!"));
		}
	});
}

bool EventLoop::prepareExitHandle()
{
	std::lock_guard<std::mutex> lock(exitMutex_);
	if (stop_)
	{
		// Already exited
		return true;
	}

	if (asyncExitHandle_)
	{
		return false;
	}

	// Init exit handle
	asyncExitHandle_ = loop_->resource<uvw::AsyncHandle>();
	asyncExitHandle_->on<uvw::AsyncEvent>([this](const uvw::AsyncEvent&, const uvw::AsyncHandle& h)
	{
		if (h.closing())
		{
			return;
		}

		{
			// Make sure no more call to this handle
			std::lock_guard<std::mutex> lock(exitMutex_);
			asyncExitHandle_.reset();
		}

		// Close all handle
		h.loop().walk([](uvw::BaseHandle& h) { h.close(); });
	});
	asyncExitHandle_->on<uvw::CloseEvent>([this](const uvw::CloseEvent&, const uvw::AsyncHandle& h)
	{
		// Make sure no more call to this handle
		std::lock_guard<std::mutex> lock(exitMutex_);
		asyncExitHandle_.reset();
	});

	return false;
}

std::shared_ptr<uvw::Loop> EventLoop::loop() const
{
	return loop_;
}

void EventLoop::run() const
{
	loop_->run();
}

void EventLoop::stopGracefully()
{
	std::lock_guard<std::mutex> lock(exitMutex_);
	stop_ = true;

	if (asyncExitHandle_ && !asyncExitHandle_->closing())
	{
		asyncExitHandle_->send();
	}
}
