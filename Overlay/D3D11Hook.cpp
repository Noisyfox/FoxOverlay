#include "D3D11Hook.h"
#include "graphic_util.h"
#include "scope_guard.hpp"

std::shared_ptr<D3D11Hook> D3D11Hook::tryHookD3D11()
{
	auto hook = std::make_shared<D3D11Hook>();
	if (hook->hook())
	{
		return hook;
	}

	return nullptr;
}

bool D3D11Hook::hook()
{
	hModuleD3D11_ = GetModuleHandle(_T("d3d11.dll"));
	if (!hModuleD3D11_)
	{
		return false;
	}

	pD3d11CreateDeviceAndSwapChain_ = reinterpret_cast<PD3D11CreateDeviceAndSwapChain>(
		GetProcAddress(hModuleD3D11_, "D3D11CreateDeviceAndSwapChain")
	);
	if (!pD3d11CreateDeviceAndSwapChain_)
	{
		return false;
	}

	// Create a hidden 2x2 window for creating new D3D11 device
	const auto hWindow = CreateStaticWindow(_T("D3D11 Hook Win"));
	if (!hWindow)
	{
		return false;
	}
	auto cleanupGuard = sg::make_scope_guard([hWindow]
	{
		DestroyWindow(hWindow);
	});

	// Create d3d11 device and swap chain
	DXGI_SWAP_CHAIN_DESC desc{};
	desc.BufferCount = 1;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.Width = 2;
	desc.BufferDesc.Height = 2;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = hWindow;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	desc.Windowed = TRUE;

	auto featureLevel = D3D_FEATURE_LEVEL_11_0;

	IUnknownPtr<IDXGISwapChain> pSwapChain;
	IUnknownPtr<ID3D11Device> pDevice;
	IUnknownPtr<ID3D11DeviceContext> pContext;

	const auto hr = pD3d11CreateDeviceAndSwapChain_(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_SINGLETHREADED, nullptr, 0, D3D11_SDK_VERSION,
		&desc,
		pSwapChain.reset().underlyingAddress(),
		pDevice.reset().underlyingAddress(),
		&featureLevel,
		pContext.reset().underlyingAddress()
	);
	if (FAILED(hr))
	{
		return false;
	}

	return hookSwapChain(pSwapChain);
}
