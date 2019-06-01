#pragma once

#include "dxgi_def.h"
#include "DXGISwapChainHook.h"

class D3D11Hook :
	public std::enable_shared_from_this<D3D11Hook>,
	public DXGISwapChainHook
{
private:
	HMODULE hModuleD3D11_{nullptr};
	PD3D11CreateDeviceAndSwapChain pD3d11CreateDeviceAndSwapChain_{nullptr};

	bool hook();
public:
	static std::shared_ptr<D3D11Hook> tryHookD3D11();
};
