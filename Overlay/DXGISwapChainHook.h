#pragma once

#include "dxgi_def.h"
#include "IUnknownPtr.h"
#include <dxgi1_2.h>

struct SwapChainHookedFunctions;

class DXGISwapChainHook
{
private:
	std::shared_ptr<SwapChainHookedFunctions> hookedFunctions_{nullptr};
protected:
	DXGISwapChainHook();
	bool hookSwapChain(IUnknownPtr<IDXGISwapChain> pSwapChain);

	HRESULT presentHook(IDXGISwapChain* swapChain, UINT SyncInterval, UINT Flags);
	HRESULT resizeBuffersHook(IDXGISwapChain* swapChain, UINT BufferCount, UINT Width, UINT Height,
	                          DXGI_FORMAT NewFormat, UINT SwapChainFlags);
	HRESULT resizeTargetHook(IDXGISwapChain* swapChain, const DXGI_MODE_DESC* pNewTargetParameters);
	HRESULT present1Hook(IDXGISwapChain1* swapChain, UINT SyncInterval, UINT PresentFlags,
	                     _In_ const DXGI_PRESENT_PARAMETERS* pPresentParameters);
};
