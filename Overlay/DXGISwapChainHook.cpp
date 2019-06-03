#include "FunctionWrapper.h"
#include "DXGISwapChainHook.h"
#include "ApiHook.h"
#include <dxgi1_2.h>

HRESULT STDMETHODCALLTYPE H_DXGISwapChainPresent(DXGISwapChainHook* data, IDXGISwapChain* swapChain, UINT a, UINT b)
{
	return 0;
}

HRESULT STDMETHODCALLTYPE H_DXGISwapChainResizeBuffers(DXGISwapChainHook* data, IDXGISwapChain* swapChain, UINT a1,
                                                       UINT a2, UINT a3, DXGI_FORMAT a4, UINT a5)
{
	return 0;
}

HRESULT STDMETHODCALLTYPE H_DXGISwapChainResizeTarget(DXGISwapChainHook* data, IDXGISwapChain* swapChain,
                                                      const DXGI_MODE_DESC* a)
{
	return 0;
}

HRESULT STDMETHODCALLTYPE H_DXGISwapChainPresent1(DXGISwapChainHook* data, IDXGISwapChain1* swapChain,
                                                  UINT SyncInterval, UINT PresentFlags,
                                                  _In_ const DXGI_PRESENT_PARAMETERS* pPresentParameters)
{
	return 0;
}

FUNC_WRAPPER_REGISTER_ENUM_TYPE(DXGI_FORMAT);

using DXGISwapChainPresentWrapperType = FuncWrapper<
	CallingConvs::kStdcall, DXGISwapChainHook*, HRESULT, IDXGISwapChain*, UINT, UINT>;
using DXGISwapChainResizeBuffersWrapperType = FuncWrapper<
	CallingConvs::kStdcall, DXGISwapChainHook*, HRESULT, IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT>;
using DXGISwapChainResizeTargetWrapperType = FuncWrapper<
	CallingConvs::kStdcall, DXGISwapChainHook*, HRESULT, IDXGISwapChain*, const DXGI_MODE_DESC*>;
using DXGISwapChainPresent1WrapperType = FuncWrapper<
	CallingConvs::kStdcall, DXGISwapChainHook*, HRESULT, IDXGISwapChain1*, UINT, UINT, _In_ const
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
		H_DXGISwapChainPresent);
	hookedFunctions_->Hooked_DXGISwapChainPresent->data(this);

	hookedFunctions_->Hooked_DXGISwapChainResizeBuffers = std::make_unique<DXGISwapChainResizeBuffersWrapperType>(
		H_DXGISwapChainResizeBuffers);
	hookedFunctions_->Hooked_DXGISwapChainResizeBuffers->data(this);

	hookedFunctions_->Hooked_DXGISwapChainResizeTarget = std::make_unique<DXGISwapChainResizeTargetWrapperType>(
		H_DXGISwapChainResizeTarget);
	hookedFunctions_->Hooked_DXGISwapChainResizeTarget->data(this);

	hookedFunctions_->Hooked_DXGISwapChainPresent1 = std::make_unique<DXGISwapChainPresent1WrapperType>(
		H_DXGISwapChainPresent1);
	hookedFunctions_->Hooked_DXGISwapChainPresent1->data(this);
}

bool DXGISwapChainHook::hookSwapChain(IUnknownPtr<IDXGISwapChain> pSwapChain)
{
	const auto pSwapChainPresentOrig = reinterpret_cast<DXGISwapChainPresentWrapperType::TargetFuncType>(
		getVFunction(pSwapChain.underlying(), 8));
	const auto pSwapChainResizeBuffersOrig = reinterpret_cast<DXGISwapChainResizeBuffersWrapperType::TargetFuncType>(
		getVFunction(pSwapChain.underlying(), 13));
	const auto pSwapChainResizeTargetOrig = reinterpret_cast<DXGISwapChainResizeTargetWrapperType::TargetFuncType>(
		getVFunction(pSwapChain.underlying(), 14));
	// TODO: hook func

	IUnknownPtr<IDXGISwapChain1> pDXGISwapChain1;
	auto hr = pSwapChain.queryInterface(pDXGISwapChain1);
	if (SUCCEEDED(hr) && pDXGISwapChain1)
	{
		OutputDebugString(_T("Is IDXGISwapChain1\n"));

		const auto pSwapChainPresent1Orig = reinterpret_cast<DXGISwapChainPresent1WrapperType::TargetFuncType>(
			getVFunction(pDXGISwapChain1.underlying(), 22));
		// TODO: hook func
	}


	return true;
}
