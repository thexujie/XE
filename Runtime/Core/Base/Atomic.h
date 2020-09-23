#pragma once

#include "Types.h"

namespace XE
{
	enum class MemoryOrder
	{
		Relaxed = 0,
		Consume,
		Acquire,
		Release,
		AcquireRelease,
		SequenceConst,
	};

	template<typename T, size_t = sizeof(T)> struct TAtomicBase;

	template<typename T>
	struct CORE_API TAtomicBase<T, 1>
	{
		using InterT = char;

		TAtomicBase() = default;
		TAtomicBase(const T & ValueIn) : Value(ValueIn) {}

		T Load(MemoryOrder Order = MemoryOrder::SequenceConst) const noexcept { return Value; }
		void Store(T Desired, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			_InterlockedExchange8(AddressAs<char, T>(Value), ValueAs<char, T>(Desired));
		}

		T FetchAdd(T Arg, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			InterT Result = _InterlockedExchangeAdd8(AddressAs<char, T>(Value), ValueAs<char, T>(Arg));
			return static_cast<T>(Result);
		}

		template<bool = IsSignedV<T>>
		T FetchSub(T Arg, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			InterT Result = _InterlockedExchangeAdd8(AddressAs<char, T>(Value), ValueAs<char, T>(-Arg));
			return static_cast<T>(Result);
		}

		T FetchAnd(T Arg, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			InterT Result = _InterlockedAnd8(AddressAs<char, T>(Value), ValueAs<char, T>(Arg));
			return static_cast<T>(Result);
		}

		T FetchOr(T Arg, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			InterT Result = _InterlockedOr8(AddressAs<char, T>(Value), ValueAs<char, T>(Arg));
			return static_cast<T>(Result);
		}

		T FetchXor(T Arg, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			InterT Result = _InterlockedXor8(AddressAs<char, T>(Value), ValueAs<char, T>(Arg));
			return static_cast<T>(Result);
		}

		T Value;
	};

	template<typename T>
	struct CORE_API TAtomicBase<T, 2>
	{
		using InterT = short;

		TAtomicBase() = default;
		TAtomicBase(const T & ValueIn) : Value(ValueIn) {}

		T Load(MemoryOrder Order = MemoryOrder::SequenceConst) const noexcept { return Value; }
		void Store(T Desired, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			_InterlockedExchange16(AddressAs<short, T>(Value), ValueAs<short, T>(Desired));
		}

		T FetchAdd(T Arg, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			InterT Result = _InterlockedExchangeAdd16(AddressAs<short, T>(Value), ValueAs<short, T>(Arg));
			return static_cast<T>(Result);
		}

		template<bool = IsSignedV<T>>
		T FetchSub(T Arg, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			InterT Result = _InterlockedExchangeAdd16(AddressAs<short, T>(Value), ValueAs<short, T>(-Arg));
			return static_cast<T>(Result);
		}

		T FetchAnd(T Arg, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			InterT Result = _InterlockedAnd16(AddressAs<short, T>(Value), ValueAs<short, T>(Arg));
			return static_cast<T>(Result);
		}

		T FetchOr(T Arg, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			InterT Result = _InterlockedOr16(AddressAs<short, T>(Value), ValueAs<short, T>(Arg));
			return static_cast<T>(Result);
		}

		T FetchXor(T Arg, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			InterT Result = _InterlockedXor16(AddressAs<short, T>(Value), ValueAs<short, T>(Arg));
			return static_cast<T>(Result);
		}

		T Value;
	};

	template<typename T>
	struct CORE_API TAtomicBase<T, 4>
	{
		using InterT = long;

		TAtomicBase() = default;
		TAtomicBase(const T & ValueIn) : Value(ValueIn) {}

		T Load(MemoryOrder Order = MemoryOrder::SequenceConst) const noexcept { return Value; }
		void Store(T Desired, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			_InterlockedExchange(AddressAs<long, T>(Value), ValueAs<long, T>(Desired));
		}

		T FetchAdd(T Arg, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			InterT Result = _InterlockedExchangeAdd(AddressAs<long, T>(Value), ValueAs<long, T>(Arg));
			return static_cast<T>(Result);
		}

		template<bool = IsSignedV<T>>
		T FetchSub(T Arg, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			InterT Result = _InterlockedExchangeAdd(AddressAs<long, T>(Value), ValueAs<long, T>(-Arg));
			return static_cast<T>(Result);
		}

		T FetchAnd(T Arg, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			InterT Result = _InterlockedAnd(AddressAs<long, T>(Value), ValueAs<long, T>(Arg));
			return static_cast<T>(Result);
		}

		T FetchOr(T Arg, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			InterT Result = _InterlockedOr(AddressAs<long, T>(Value), ValueAs<long, T>(Arg));
			return static_cast<T>(Result);
		}

		T FetchXor(T Arg, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			InterT Result = _InterlockedXor(AddressAs<long, T>(Value), ValueAs<long, T>(Arg));
			return static_cast<T>(Result);
		}

		T Value;
	};

	template<typename T>
	struct CORE_API TAtomicBase<T, 8>
	{
		using InterT = long long;

		TAtomicBase() = default;
		TAtomicBase(const T & ValueIn) : Value(ValueIn) {}

		T Load(MemoryOrder Order = MemoryOrder::SequenceConst) const noexcept { return Value; }
		void Store(T Desired, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			_InterlockedExchange64(AddressAs<long long, T>(Value), ValueAs<long long, T>(Desired));
		}


		T FetchAdd(T Arg, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			InterT Result = _InterlockedExchangeAdd64(AddressAs<long long, T>(Value), ValueAs<long long, T>(Arg));
			return static_cast<T>(Result);
		}

		template<bool = IsSignedV<T>>
		T FetchSub(T Arg, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			InterT Result = _InterlockedExchangeAdd64(AddressAs<long long, T>(Value), ValueAs<long long, T>(-Arg));
			return static_cast<T>(Result);
		}

		T FetchAnd(T Arg, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			InterT Result = _InterlockedAnd64(AddressAs<long long, T>(Value), ValueAs<long long, T>(Arg));
			return static_cast<T>(Result);
		}

		T FetchOr(T Arg, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			InterT Result = _InterlockedOr64(AddressAs<long long, T>(Value), ValueAs<long long, T>(Arg));
			return static_cast<T>(Result);
		}

		T FetchXor(T Arg, MemoryOrder Order = MemoryOrder::SequenceConst) noexcept
		{
			InterT Result = _InterlockedXor64(AddressAs<long long, T>(Value), ValueAs<long long, T>(Arg));
			return static_cast<T>(Result);
		}

		T Value;
	};
	
	template<typename T>
	class CORE_API TAtomic : public TAtomicBase<T>
	{
	public:
		TAtomic() = default;
		TAtomic(const T & ValueIn) : TAtomicBase<T>(ValueIn) {}

		template<bool = IsIntegralV<T>>
		T operator++() noexcept { return TAtomicBase<T>::FetchAdd(1) + 1; }

		template<bool = IsIntegralV<T>>
		T operator++(int) noexcept { return TAtomicBase<T>::FetchAdd(1); }

		template<bool = IsIntegralV<T> && IsSignedV<T>>
		T operator--() noexcept { return TAtomicBase<T>::FetchSub(1) - 1; }

		template<bool = IsIntegralV<T> && IsSignedV<T>>
		T operator--(int) noexcept { return TAtomicBase<T>::FetchSub(1); }

		operator T() const { return TAtomicBase<T>::Load(MemoryOrder::SequenceConst); }
	};

	using FAtomicBool = TAtomic<bool>;
	using FAtomicInt32 = TAtomic<int32_t>;
	using FAtomicUInt32 = TAtomic<uint32_t>;
	using FAtomicInt64 = TAtomic<int64_t>;
	using FAtomicUInt64 = TAtomic<uint64_t>;
	using FAtomicFloat32 = TAtomic<float32_t>;
	using FAtomicFloat64 = TAtomic<float64_t>;

	template class CORE_API TAtomic<bool>;
	template class CORE_API TAtomic<int32_t>;
	template class CORE_API TAtomic<uint32_t>;
	template class CORE_API TAtomic<int64_t>;
	template class CORE_API TAtomic<uint64_t>;
	template class CORE_API TAtomic<float32_t>;
	template class CORE_API TAtomic<float64_t>;

	namespace Atomics
	{
		template<Integral T>
		static T IncFetch(T & Value)
		{
			if constexpr (sizeof(T) == 4)
			{
				long Result = _InterlockedIncrement(AddressAs<long, T>(Value));
				return ValueAs<T, long>(Result);
			}
			else if constexpr (sizeof(T) == 8)
			{
				__int64 Result = _InterlockedIncrement64(AddressAs<__int64, T>(Value));
				return ValueAs<T, __int64>(Result);
			}
			else
				throw EError::Args;
		}

		template<Integral T>
		static T DecFetch(T & Value)
		{
			if constexpr (sizeof(T) == 4)
			{
				long Result = _InterlockedDecrement(AddressAs<long, T>(Value));
				return ValueAs<T, long>(Result);
			}
			else if constexpr (sizeof(T) == 8)
			{
				__int64 Result = _InterlockedDecrement64(AddressAs<__int64, T>(Value));
				return ValueAs<T, __int64>(Result);
			}
			else
				throw EError::Args;
		}

		template<Integral T>
		static T FetchExchange(T & Value, T NewValue)
		{
			if constexpr (sizeof(T) == 4)
			{
				long Result = _InterlockedExchange(AddressAs<long, T>(Value), NewValue);
				return ValueAs<T, long>(Result);
			}
			else if constexpr (sizeof(T) == 8)
			{
				__int64 Result = _InterlockedExchange64(AddressAs<__int64, T>(Value), NewValue);
				return ValueAs<T, __int64>(Result);
			}
			else
				throw EError::Args;
		}
	}
}
