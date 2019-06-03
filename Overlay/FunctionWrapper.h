#pragma once

#include <vector>

#define ASMJIT_STATIC
#include <asmjit/asmjit.h>

#include <stdexcept>

#define FUNC_WRAPPER_DEFINE_CALL_CONV(NAME, ASMJIT_CONV) \
	struct NAME { enum { kCallConv = asmjit::CallConv::ASMJIT_CONV }; };

struct CallingConvs
{
	FUNC_WRAPPER_DEFINE_CALL_CONV(kCdecl, kIdHostCDecl);

	FUNC_WRAPPER_DEFINE_CALL_CONV(kStdcall, kIdHostStdCall);
};

#undef FUNC_WRAPPER_DEFINE_CALL_CONV

template <
	typename CallConv,
	typename UserDataType,
	typename ReturnType,
	typename... ArgumentType
>
struct FuncType
{
	// Don't provide anything if not specialized.
};

#define FUNC_WRAPPER_DEFINE_FUNC_TYPE(T, CALL_CONV) \
	template<typename UserDataType, typename ReturnType, typename... ArgumentType> \
	struct FuncType<T, UserDataType, ReturnType, ArgumentType...> \
	{ \
		using TargetFuncType = ReturnType(CALL_CONV *)(UserDataType, ArgumentType ...); \
		using WrappedFuncType = ReturnType(CALL_CONV *)(ArgumentType ...); \
	}

FUNC_WRAPPER_DEFINE_FUNC_TYPE(CallingConvs::kCdecl, __cdecl);

FUNC_WRAPPER_DEFINE_FUNC_TYPE(CallingConvs::kStdcall, __stdcall);

#undef FUNC_WRAPPER_DEFINE_FUNC_TYPE

template <
	typename CallConv,
	typename UserDataType,
	typename ReturnType,
	typename... ArgumentType
>
class FuncWrapper
{
	using TargetFuncType = typename FuncType<CallConv, UserDataType, ReturnType, ArgumentType...>::TargetFuncType;
	using WrappedFuncType = typename FuncType<CallConv, UserDataType, ReturnType, ArgumentType...>::WrappedFuncType;

private:
	const TargetFuncType targetFn_;
	WrappedFuncType wrappedFn_{nullptr};
	asmjit::JitRuntime rt_;

	UserDataType userData_{};

public:
	FuncWrapper(
		TargetFuncType pTargetFn
	) : targetFn_(pTargetFn)
	{
		// Initialize CodeHolder
		asmjit::CodeHolder code;
		code.init(rt_.getCodeInfo());

		// Create and attach X86Compiler to `code`.
		asmjit::X86Compiler cc(&code);

		// Begin the forward function
		asmjit::FuncSignatureT<ReturnType, ArgumentType...> sig(CallConv::kCallConv);
		cc.addFunc(sig);

		// Prepare arguments
		auto argCount = sig.getArgCount();
		std::vector<asmjit::X86Reg> argRegs;
		for (auto i = 0u; i < argCount; ++i)
		{
			auto argTypeId = sig.getArg(i);

			auto argReg = cc.newReg(argTypeId);
			cc.setArg(i, argReg);
			cc.spill(argReg);

			argRegs.push_back(argReg);
		}

		// Read data
		// TODO: Support XMM
		auto dataRegBase = cc.newIntPtr("data_base");
		cc.mov(dataRegBase, reinterpret_cast<uint64_t>(&userData_));
		auto dataReg = cc.newGpReg(asmjit::TypeIdOf<UserDataType>::kTypeId);
		auto dataMem = asmjit::x86::ptr(dataRegBase, 0, sizeof(UserDataType));
		cc.mov(dataReg, dataMem);

		// Create call to target function
		auto c = cc.call(reinterpret_cast<uint64_t>(targetFn_),
		                 asmjit::FuncSignatureT<ReturnType, UserDataType, ArgumentType...>(CallConv::kCallConv));

		// Pass data
		c->setArg(0, dataReg);

		// Forward arguments
		for (auto i = 0u; i < argCount; ++i)
		{
			c->setArg(i + 1, argRegs[i]);
		}

		// Forward return val
		auto retReg = cc.newReg(asmjit::TypeIdOf<ReturnType>::kTypeId);
		c->setRet(0, retReg);
		cc.addRet(retReg, asmjit::Operand());

		// Complete func
		cc.endFunc();
		cc.finalize();

		auto err = rt_.add(&wrappedFn_, &code);
		if (err)
		{
			OutputDebugStringA("Error!");
			wrappedFn_ = nullptr;

			throw std::runtime_error("Unable to create function wrapper!");
		}
	}

	~FuncWrapper()
	{
		if (wrappedFn_ != nullptr)
		{
			rt_.release(wrappedFn_);
			wrappedFn_ = nullptr;
		}
	}

	void data(UserDataType data)
	{
		userData_ = data;
	}

	UserDataType data() const
	{
		return userData_;
	}

	WrappedFuncType func() const
	{
		return wrappedFn_;
	}
};
