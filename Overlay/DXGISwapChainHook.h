#pragma once

#include "dxgi_def.h"
#include "IUnknownPtr.h"

class DXGISwapChainHook
{
protected:
	bool hookSwapChain(IUnknownPtr<IDXGISwapChain> pSwapChain);
};
