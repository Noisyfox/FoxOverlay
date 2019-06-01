#include "DXGIHook.h"
#include "Session.h"
#include "D3D11Hook.h"

std::shared_ptr<DXGIHook> DXGIHook::tryHookDXGI(std::shared_ptr<Session> session)
{
	const auto hDxgi = GetModuleHandle(_T("dxgi.dll"));
	if (!hDxgi)
	{
		return nullptr;
	}

	auto pD3d11Hook = D3D11Hook::tryHookD3D11();
	if(!pD3d11Hook)
	{
		return nullptr;
	}

	return nullptr;
}
