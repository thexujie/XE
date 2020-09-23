#pragma once

#include "Types.h"

namespace XE
{
	template <typename PromiseT = void>
	struct TCoroutineHandle;

	template <>
	struct TCoroutineHandle<void> 
	{
		TCoroutineHandle() noexcept {}
		TCoroutineHandle(nullptr_t) noexcept {}
		auto operator <=> (const TCoroutineHandle &)const = default;

		template<typename StdCoroutineHandleT>
		TCoroutineHandle(const StdCoroutineHandleT & StdCoroutineHandle) noexcept :Pointer((FResumableFramePrefix *)StdCoroutineHandle.address()) {}

		TCoroutineHandle & operator=(nullptr_t) noexcept 
		{
			Pointer = nullptr;
			return *this;
		}

		static TCoroutineHandle from_address(void * _Addr) noexcept 
		{
			TCoroutineHandle Result;
			Result.Pointer = static_cast<FResumableFramePrefix *>(_Addr);
			return Result;
		}

		void * address() const noexcept { return Pointer; }

		void operator()() const noexcept { resume(); }

		explicit operator bool() const noexcept { return Pointer != nullptr; }

		void resume() const { _coro_resume(Pointer); }
		void destroy() { _coro_destroy(Pointer); }

		bool done() const { return Pointer->_Index == 0; }

		struct FResumableFramePrefix 
		{
			using ResumeFunT = void(__cdecl *)(void *);

			ResumeFunT _Fn;
			uint16_t _Index;
			uint16_t _Flags;
		};

	protected:
		FResumableFramePrefix * Pointer = nullptr;
	};

	template <typename PromiseT>
	struct TCoroutineHandle : TCoroutineHandle<> 
	{
		using TCoroutineHandle<>::TCoroutineHandle;

		auto operator <=> (const TCoroutineHandle &) const = default;

		static TCoroutineHandle from_promise(PromiseT & Promise) noexcept 
		{
			auto FramePointer = reinterpret_cast<char *>(AddressOf(Promise)) + AlignmentSize;
			TCoroutineHandle<PromiseT> Result;
			Result.Pointer = reinterpret_cast<FResumableFramePrefix *>(FramePointer);
			return Result;
		}

		static TCoroutineHandle from_address(void * Address) noexcept 
		{
			TCoroutineHandle Result;
			Result.Pointer = static_cast<FResumableFramePrefix *>(Address);
			return Result;
		}

		TCoroutineHandle & operator=(nullptr_t) noexcept { Pointer = nullptr; return *this; }

		static const size_t AlignmentRequire = sizeof(void *) * 2;
		static const size_t AlignmentSize = IsEmptyV<PromiseT> ? 0 : ((sizeof(PromiseT) + AlignmentRequire - 1) & ~(AlignmentRequire - 1));

		PromiseT & promise() const noexcept 
		{
			return *const_cast<PromiseT *>(reinterpret_cast<PromiseT const *>(reinterpret_cast<char const *>(Pointer) - AlignmentSize));
		}
	};


	struct SuspendIf 
	{
		bool IsReady;

		explicit SuspendIf(bool _Condition) noexcept : IsReady(!_Condition) {}
		bool await_ready() noexcept { return IsReady; }
		void await_suspend(TCoroutineHandle<>) noexcept {}
		void await_resume() noexcept {}
	};

	struct SuspendAlways
	{
		bool await_ready() noexcept { return false; }
		void await_suspend(TCoroutineHandle<>) noexcept {}
		void await_resume() noexcept {}
	};

	struct SuspendNever
	{
		bool await_ready() noexcept { return true; }
		void await_suspend(TCoroutineHandle<>) noexcept {}
		void await_resume() noexcept {}
	};
}
