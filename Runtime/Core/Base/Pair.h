#pragma once

#include "Types.h"

namespace XE
{
	template <typename KeyT, typename ValueT>
	struct TPair
	{
		TPair() = default;
		TPair(const TPair &) = default;
		TPair(TPair &&) noexcept = default;
		TPair & operator=(const TPair &) = default;
		TPair & operator=(TPair &&) = default;
		auto operator <=>(const TPair &) const = default;

		TPair(const KeyT & Key_) : Key(Key_), Value{} {}
		TPair(const KeyT & Key_, const ValueT & Value_) : Key(Key_), Value(Value_) {}
		TPair(KeyT && Key_, ValueT && Value_) : Key(Forward<KeyT>(Key_)), Value(Forward<ValueT>(Value_)) {}

		//template<typename KeyT2, typename ValueT2>
		//TPair(KeyT2 && Key_, ValueT2 && Value_) : First(Forward<KeyT2>(Key_)), Second(Forward<ValueT2>(Value_)) {}

		//template<typename KeyT2, typename ValueT2>
		//TPair(TPair<KeyT2, ValueT2> && Pair) : First(Forward<KeyT2>(Pair.First)), Second(Forward<ValueT2>(Pair.Second)) {}
		
		KeyT Key;
		ValueT Value;
	};

	template<typename KeyT, typename ValueT>
	TPair<KeyT, ValueT> MakePair(const KeyT & First, const ValueT & Second)
	{
		return TPair<KeyT, ValueT>(First, Second);
	}

	template<typename KeyT, typename ValueT>
	struct THash<TPair<KeyT, ValueT>>
	{
		uintx_t operator()(const TPair<KeyT, ValueT> & Pair) const noexcept
		{
			uintx_t HashValue = HashJoin(Pair.Key);
			HashValue = DEF_BASE_X * HashValue ^ HashJoin(Pair.Value);
			return HashValue;
		}
	};
}
