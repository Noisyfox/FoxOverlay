#pragma once

#include <uvw.hpp>
#include <mutex>

class EventLoop
{
private:
	std::mutex exitMutex_;

	bool stop_ = false;

	std::shared_ptr<uvw::Loop> loop_;
	std::shared_ptr<uvw::AsyncHandle> asyncExitHandle_ = nullptr;

public:
	EventLoop();

	/**
	 * If returned false then run() must be called to release resources.
	 */
	bool prepareExitHandle();

	std::shared_ptr<uvw::Loop> loop() const;

	void run() const;

	void stopGracefully();
};
