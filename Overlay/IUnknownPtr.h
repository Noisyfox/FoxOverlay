#pragma once

#include "framework.h"
#include <Unknwn.h>

template <
	class T,
	typename = std::enable_if_t<std::is_base_of<IUnknown, T>::value>
>
class IUnknownPtr
{
private:
	T* ptr_{nullptr};

public:
	IUnknownPtr() = default;

	explicit IUnknownPtr(T* pUnknown): ptr_(pUnknown)
	{
		if (ptr_)
		{
			ptr_->AddRef();
		}
	}

	IUnknownPtr(const IUnknownPtr<T>& other) : IUnknownPtr(other.ptr_)
	{
	}

	~IUnknownPtr()
	{
		reset();
	}

	IUnknownPtr(IUnknownPtr<T>&& other) noexcept
		: ptr_(std::exchange(other.ptr_, nullptr))
	{
	}

	IUnknownPtr& operator=(T* const pNewVal)
	{
		if (pNewVal)
		{
			pNewVal->AddRef();
		}
		reset();

		ptr_ = pNewVal;
		return *this;
	}

	IUnknownPtr& operator=(const IUnknownPtr<T>& other)
	{
		if (this == &other)
			return *this;

		return operator=(other.ptr_);
	}

	IUnknownPtr& operator=(IUnknownPtr<T>&& other) noexcept
	{
		if (this == &other)
			return *this;

		ptr_ = std::exchange(other.ptr_, nullptr);

		return *this;
	}

	operator bool() const
	{
		return ptr_ != nullptr;
	}

	operator T*() const
	{
		return ptr_;
	}

	T& operator*() const
	{
		return *ptr_;
	}

	T* operator->() const
	{
		return ptr_;
	}

	T* underlying() const
	{
		return ptr_;
	}

	T** underlyingAddress()
	{
		return &ptr_;
	}

	template <class U>
	HRESULT queryInterface(IUnknownPtr<U>& dest) const
	{
		if (ptr_ == nullptr)
		{
			return E_POINTER;
		}

		return ptr_->QueryInterface(__uuidof(U), reinterpret_cast<void**>(dest.reset().underlyingAddress()));
	}

	IUnknownPtr& reset()
	{
		if (ptr_ != nullptr)
		{
			ptr_->Release();
			ptr_ = nullptr;
		}

		return *this;
	}
};
