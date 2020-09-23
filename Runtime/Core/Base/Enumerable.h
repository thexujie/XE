#pragma once

#include "Types.h"
#include "Resumable.h"

namespace XE
{
	template<typename ElementT>
	struct TEnumerable
	{
		struct PromiseType
		{
			ElementT const * CurrentValue;
#ifdef _CPPUNWIND
			// TRANSITION, VSO-967814
			std::exception_ptr ExceptionPointer;
#endif // _CPPUNWIND

			auto get_return_object() { return TEnumerable{ *this }; }
			SuspendAlways initial_suspend() noexcept { return {}; }
			SuspendAlways final_suspend() noexcept { return {}; }

#ifdef _CPPUNWIND
			void unhandled_exception() noexcept { ExceptionPointer = _STD current_exception(); }
#endif // _CPPUNWIND

			void _Rethrow_if_exception()
			{
#ifdef _CPPUNWIND
				if (ExceptionPointer) { _STD rethrow_exception(ExceptionPointer); }
#endif // _CPPUNWIND
			}

			auto yield_value(ElementT const & _Value)
			{
				CurrentValue = AddressOf(_Value);
				return SuspendAlways{};
			}

			void return_void() {}

			template<typename ThatT>
			ThatT && await_transform(ThatT && Whatever)
			{
				static_assert(FalseTypeV<ThatT>,
					"co_await is not supported in coroutines of type std::experimental::generator");
				return Forward<ThatT>(Whatever);
			}

			//static void * operator new(size_t _Size) {
			//	_Alloc_char _Al;
			//	return allocator_traits<_Alloc_char>::allocate(_Al, _Size);
			//}

			//static void operator delete(void * Pointer, size_t _Size) noexcept {
			//	_Alloc_char _Al;
			//	return allocator_traits<_Alloc_char>::deallocate(_Al, static_cast<char *>(Pointer), _Size);
			//}
		};

		using promise_type = PromiseType;

		struct Iterator
		{
			//using iterator_category = std::input_iterator_tag;
			//using difference_type = ptrdiff_t;
			using ValueType = ElementT;
			using ReferenceType = ElementT const&;
			using PointerType = ElementT const*;

			TCoroutineHandle<PromiseType> CoroutineHandle = nullptr;

			Iterator() = default;
			Iterator(nullptr_t) : CoroutineHandle(nullptr) {}
			Iterator(TCoroutineHandle<PromiseType> CoroutineHandle_) : CoroutineHandle(CoroutineHandle_) {}

			Iterator & operator++()
			{
				CoroutineHandle.resume();
				if (CoroutineHandle.done())
				{
					Exchange(CoroutineHandle, {}).promise()._Rethrow_if_exception();
				}

				return *this;
			}

			void operator++(int) { ++ *this; }
			bool operator==(Iterator const & Another) const { return CoroutineHandle == Another.CoroutineHandle; }
			bool operator!=(Iterator const & Another) const { return !(*this == Another); }

			ReferenceType operator*() const { return *CoroutineHandle.promise().CurrentValue; }
			PointerType operator->() const { return CoroutineHandle.promise().CurrentValue; }
		};

		Iterator begin()
		{
			if (CoroutineHandle)
			{
				CoroutineHandle.resume();
				if (CoroutineHandle.done())
				{
					CoroutineHandle.promise()._Rethrow_if_exception();
					return { nullptr };
				}
			}

			return { CoroutineHandle };
		}

		Iterator end() { return { nullptr }; }

		explicit TEnumerable(PromiseType & Promise) : CoroutineHandle(TCoroutineHandle<PromiseType>::from_promise(Promise)) {}

		TEnumerable() = default;

		TEnumerable(TEnumerable const &) = delete;

		TEnumerable & operator=(TEnumerable const &) = delete;

		TEnumerable(TEnumerable && Another) noexcept : CoroutineHandle(Another.CoroutineHandle)
		{
			Another.CoroutineHandle = nullptr;
		}

		TEnumerable & operator=(TEnumerable && Another) noexcept
		{
			if (this != AddressOf(Another))
			{
				CoroutineHandle = Another.CoroutineHandle;
				Another.CoroutineHandle = nullptr;
			}
			return *this;
		}

		~TEnumerable()
		{
			if (CoroutineHandle)
			{
				CoroutineHandle.destroy();
			}
		}

	private:
		TCoroutineHandle<PromiseType> CoroutineHandle = nullptr;
	};
}
