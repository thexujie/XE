#pragma once

#include "Types.h"

namespace XE
{
	const uint32_t DEF_BASE_32 = 0x01000193u;
	const uint32_t DEF_SEED_32 = 0x811C9DC5u;
	const uint64_t DEF_BASE_64 = 0x0100019301000193u;
	const uint64_t DEF_SEED_64 = 0x811C9DC5811C9DC5u;

#ifdef XE_X64
	const uint64_t DEF_BASE_X = DEF_BASE_64;
	const uint64_t DEF_SEED_X = DEF_SEED_64;
#else
	const uint32_t DEF_BASE_X = DEF_BASE_32;
	const uint32_t DEF_SEED_X = DEF_SEED_32;
#endif

	template<typename ValT>
	uint32_t Hash8(const ValT & Value, uint32_t HashValueBase = DEF_SEED_32)
	{
		static_assert(sizeof(ValT) == 1);
		uint32_t UI32Value = *reinterpret_cast<const uint8_t *>(&Value);
		return DEF_BASE_32 * HashValueBase ^ UI32Value;
	}
	
	template<typename ValT>
	uint32_t Hash16(const ValT & Value, uint32_t HashValueBase = DEF_SEED_32)
	{
		static_assert(sizeof(ValT) == 2);
		uint32_t UI32Value = *reinterpret_cast<const uint16_t *>(&Value);
		return DEF_BASE_32 * HashValueBase ^ UI32Value;
	}
	
	template<typename ValT>
	uint32_t Hash32(const ValT & Value, uint32_t HashValueBase = DEF_SEED_32)
	{
		static_assert(sizeof(ValT) == 4);
		uint32_t UI32Value = *reinterpret_cast<const uint32_t*>(&Value);
		return DEF_BASE_32 * HashValueBase ^ UI32Value;
	}

	template<typename ValT>
	uint64_t Hash64(const ValT & Value, uint64_t HashValueBase = DEF_SEED_64)
	{
		static_assert(sizeof(ValT) == 8);
		uint64_t UI64Value = *reinterpret_cast<const uint64_t *>(&Value);
		return DEF_BASE_64 * HashValueBase ^ UI64Value;
	}

	template<typename ValT>
	uintx_t HashX(const ValT & Value, uintx_t HashValueBase = DEF_SEED_X)
	{
		static_assert(sizeof(ValT) == sizeof(intx_t));
		uintx_t UI64Value = *reinterpret_cast<const uintx_t *>(&Value);
		return DEF_BASE_X * HashValueBase ^ UI64Value;
	}

	inline uintx_t HashBool(const bool & Value, uintx_t HashValueBase = DEF_SEED_X)
	{
		uintx_t UIXValue = Value ? UIntXMax : 0;
		return DEF_BASE_X * HashValueBase ^ UIXValue;
	}

	CORE_API uintx_t HashBytes(const byte_t * Bytes, size_t Size, uintx_t HashValueBase = DEF_SEED_X);
	
	inline uintx_t HashString(const char * String, size_t Size, uintx_t HashValueBase = DEF_SEED_X)
	{
		return HashBytes(reinterpret_cast<const byte_t*>(String), Size * sizeof(char), HashValueBase);
	}

	inline uintx_t HashString(const char8_t * String, size_t Size, uintx_t HashValueBase = DEF_SEED_X)
	{
		return HashBytes(reinterpret_cast<const byte_t *>(String), Size * sizeof(char8_t), HashValueBase);
	}

	inline uintx_t HashString(const char16_t * String, size_t Size, uintx_t HashValueBase = DEF_SEED_X)
	{
		return HashBytes(reinterpret_cast<const byte_t *>(String), Size * sizeof(char16_t), HashValueBase);
	}

	inline uintx_t HashString(const char32_t * String, size_t Size, uintx_t HashValueBase = DEF_SEED_X)
	{
		return HashBytes(reinterpret_cast<const byte_t *>(String), Size * sizeof(char32_t), HashValueBase);
	}

	template<typename T>
	struct THash
	{
		template<typename = EnableIfT<IsTriviallyV<T> || IsEnumV<T>>>
		uintx_t operator()(const T & Value) const noexcept
		{
			using Type = DecayT<T>;
			if constexpr (IsTriviallyV<Type>)
				return HashBytes(reinterpret_cast<const byte_t *>(&Value), sizeof(T));
			else if constexpr (IsEnumV<Type>)
			{
				using UnderlyingT = UnderlyingTypeT<T>;
				return THash<UnderlyingT>()(UnderlyingT(Value));
			}
			else
			{
				static_assert(IsEnumV<Type>, "Only trivially type or enum has a default THash<T>.");
				return 0;
			}
		}
	};
	
	template<typename T>
	concept Hashable = requires(T t) { {t.Hash() } ->ConvertibleTo<uintx_t>; };

	template<Hashable T>
	struct THash<T>
	{
		uintx_t operator()(const T & Value) const noexcept
		{
			return Value.Hash();
		}
	};

	template<typename T>
	uintx_t HashJoin(const T & Value)
	{
		return THash<T>()(Value);
	}

	template<typename T, typename ...ArgsT>
	uintx_t HashJoin(const T & Value, const ArgsT &...Args)
	{
		uintx_t HashValue = HashJoin<T>(Value);
		return DEF_BASE_X * HashValue ^ HashJoin(Args...);
	}

	template<typename T, typename ...ArgsT>
	uintx_t HashCombine(const T & HashValue, const ArgsT &...Args)
	{
		return DEF_BASE_X * HashValue ^ HashJoin(Args...);
	}
	
	template<> struct THash<bool> { uintx_t operator()(bool Value) const noexcept { return HashBool(Value); } };
	template<> struct THash<int8_t> { uintx_t operator()(int8_t Value) const noexcept { return Hash8(Value); } };
	template<> struct THash<uint8_t> { uintx_t operator()(uint8_t Value) const noexcept { return Hash8(Value); } };
	template<> struct THash<int16_t> { uintx_t operator()(int16_t Value) const noexcept { return Hash16(Value); } };
	template<> struct THash<uint16_t> { uintx_t operator()(uint16_t Value) const noexcept { return Hash16(Value); } };
	template<> struct THash<int32_t> { uintx_t operator()(int32_t Value) const noexcept { return Hash32(Value); } };
	template<> struct THash<uint32_t> { uintx_t operator()(uint32_t Value) const noexcept { return Hash32(Value); } };
	template<> struct THash<int64_t> { uintx_t operator()(int64_t Value) const noexcept { return Hash64(Value); } };
	template<> struct THash<uint64_t> { uintx_t operator()(uint64_t Value) const noexcept { return Hash64(Value); } };
	template<> struct THash<float32_t> { uintx_t operator()(float32_t Value) const noexcept { return Hash32(Value); } };
	template<> struct THash<float64_t> { uintx_t operator()(float64_t Value) const noexcept { return Hash64(Value); } };

	template<> struct THash<char> { uintx_t operator()(char Value) const noexcept { return Hash8(Value); } };
	template<> struct THash<char8_t> { uintx_t operator()(char8_t Value) const noexcept { return Hash8(Value); } };
	template<> struct THash<char16_t> { uintx_t operator()(char16_t Value) const noexcept { return Hash16(Value); } };
}

