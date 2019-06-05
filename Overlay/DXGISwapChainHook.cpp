#include "FunctionWrapper.h"
#include "DXGISwapChainHook.h"
#include "ApiHook.h"

FUNC_WRAPPER_REGISTER_ENUM_TYPE(DXGI_FORMAT);

using DXGISwapChainPresentWrapperType = MemberFuncWrapper<
	CallingConvs::kStdcall, DXGISwapChainHook, HRESULT, IDXGISwapChain*, UINT, UINT>;
using DXGISwapChainResizeBuffersWrapperType = MemberFuncWrapper<
	CallingConvs::kStdcall, DXGISwapChainHook, HRESULT, IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT>;
using DXGISwapChainResizeTargetWrapperType = MemberFuncWrapper<
	CallingConvs::kStdcall, DXGISwapChainHook, HRESULT, IDXGISwapChain*, const DXGI_MODE_DESC*>;
using DXGISwapChainPresent1WrapperType = MemberFuncWrapper<
	CallingConvs::kStdcall, DXGISwapChainHook, HRESULT, IDXGISwapChain1*, UINT, UINT, _In_ const
	DXGI_PRESENT_PARAMETERS*>;

struct SwapChainHookedFunctions
{
	std::unique_ptr<DXGISwapChainPresentWrapperType> Hooked_DXGISwapChainPresent{nullptr};
	std::unique_ptr<DXGISwapChainResizeBuffersWrapperType> Hooked_DXGISwapChainResizeBuffers{nullptr};
	std::unique_ptr<DXGISwapChainResizeTargetWrapperType> Hooked_DXGISwapChainResizeTarget{nullptr};
	std::unique_ptr<DXGISwapChainPresent1WrapperType> Hooked_DXGISwapChainPresent1{nullptr};
};

DXGISwapChainHook::DXGISwapChainHook()
{
	// Init function wrappers
	hookedFunctions_ = std::make_shared<SwapChainHookedFunctions>();

	hookedFunctions_->Hooked_DXGISwapChainPresent = std::make_unique<DXGISwapChainPresentWrapperType>(
		this, &DXGISwapChainHook::presentHook);
	hookedFunctions_->Hooked_DXGISwapChainResizeBuffers = std::make_unique<DXGISwapChainResizeBuffersWrapperType>(
		this, &DXGISwapChainHook::resizeBuffersHook);
	hookedFunctions_->Hooked_DXGISwapChainResizeTarget = std::make_unique<DXGISwapChainResizeTargetWrapperType>(
		this, &DXGISwapChainHook::resizeTargetHook);
	hookedFunctions_->Hooked_DXGISwapChainPresent1 = std::make_unique<DXGISwapChainPresent1WrapperType>(
		this, &DXGISwapChainHook::present1Hook);
}

bool DXGISwapChainHook::hookSwapChain(IUnknownPtr<IDXGISwapChain> pSwapChain)
{
	const auto pSwapChainPresentOrig = reinterpret_cast<DXGISwapChainPresentWrapperType::ExposedFuncType>(
		getVFunction(pSwapChain.underlying(), 8));
	const auto pSwapChainResizeBuffersOrig = reinterpret_cast<DXGISwapChainResizeBuffersWrapperType::ExposedFuncType>(
		getVFunction(pSwapChain.underlying(), 13));
	const auto pSwapChainResizeTargetOrig = reinterpret_cast<DXGISwapChainResizeTargetWrapperType::ExposedFuncType>(
		getVFunction(pSwapChain.underlying(), 14));
	// TODO: hook func

	IUnknownPtr<IDXGISwapChain1> pDXGISwapChain1;
	auto hr = pSwapChain.queryInterface(pDXGISwapChain1);
	if (SUCCEEDED(hr) && pDXGISwapChain1)
	{
		OutputDebugString(_T("Is IDXGISwapChain1\n"));

		const auto pSwapChainPresent1Orig = reinterpret_cast<DXGISwapChainPresent1WrapperType::ExposedFuncType>(
			getVFunction(pDXGISwapChain1.underlying(), 22));
		// TODO: hook func
	}

	return true;
}

HRESULT DXGISwapChainHook::presentHook(IDXGISwapChain* swapChain, UINT SyncInterval, UINT Flags)
{
	return 0;
}

HRESULT DXGISwapChainHook::resizeBuffersHook(IDXGISwapChain* swapChain, UINT BufferCount, UINT Width, UINT Height,
                                             DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
	return 0;
}

HRESULT DXGISwapChainHook::resizeTargetHook(IDXGISwapChain* swapChain, const DXGI_MODE_DESC* pNewTargetParameters)
{
	return 0;
}

HRESULT DXGISwapChainHook::present1Hook(IDXGISwapChain1* swapChain, UINT SyncInterval, UINT PresentFlags,
                                        _In_ const DXGI_PRESENT_PARAMETERS* pPresentParameters)
{
	return 0;
}
