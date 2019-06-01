#pragma once

#include "framework.h"

class Session;

class DXGIHook : public std::enable_shared_from_this<DXGIHook>
{
public:
	static std::shared_ptr<DXGIHook> tryHookDXGI(std::shared_ptr<Session> session);
};
