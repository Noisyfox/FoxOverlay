#pragma once

#include "dxgi_def.h"
#include "IUnknownPtr.h"

struct SwapChainHookedFunctions;

class DXGISwapChainHook
{
private:
	std::shared_ptr<SwapChainHookedFunctions> hookedFunctions_{nullptr};
protected:
	DXGISwapChainHook();
	bool hookSwapChain(IUnknownPtr<IDXGISwapChain> pSwapChain);
};
