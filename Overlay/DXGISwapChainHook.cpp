#include "DXGISwapChainHook.h"

bool DXGISwapChainHook::hookSwapChain(IUnknownPtr<IDXGISwapChain> pSwapChain)
{
	return true;
}
