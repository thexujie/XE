#pragma once

#include "Vector.h"

namespace XE
{
	template<typename KeyT, typename ValueT>
	class TLinearUnorderedMap : protected TVector<TPair<KeyT, ValueT>>
	{
		using BaseT = TVector<TPair<KeyT, ValueT>>;
		using PairT = TPair<KeyT, ValueT>;
		using Iterator = PairT *;
		using ConstIterator = const PairT *;
	public:
		Iterator InsertOrAssign(const KeyT & Key, const ValueT & Value)
		{
			for (size_t Index = 0; Index < BaseT::Size; ++Index)
			{
				if (BaseT::Data[Index].Key == Key)
				{
					BaseT::Data[Index].Value = Value;
					return BaseT::Data + Index;
				}
			}

			BaseT::Add({Key, Value});
			return BaseT::Data + (BaseT::Size - 1);
		}

		Iterator Find(const KeyT & Key)
		{
			for (size_t Index = 0; Index < BaseT::Size; ++Index)
			{
				if (BaseT::Data[Index].Key == Key)
				{
					return BaseT::Data + Index;
				}
			}
			return End();
		}

		Iterator Begin() { return BaseT::Begin(); }
		ConstIterator Begin() const { return BaseT::Begin(); }
		Iterator End() { return BaseT::End(); }
		ConstIterator End() const { return BaseT::End(); }

		ConstIterator Find(const KeyT & Key) const
		{
			for (size_t Index = 0; Index < BaseT::Size; ++Index)
			{
				if (BaseT::Data[Index].Key == Key)
				{
					return BaseT::Data + Index;
				}
			}
			return End();
		}

		bool Contains(const KeyT & Key) const { return Find(Key) != End(); }
		
		ValueT & operator[](const KeyT & Key)
		{
			Iterator Iter = Find(Key);
			if (Iter == End())
				return BaseT::Add({ Key, ValueT{} }).Value;
			return Iter->Value;
		}

		const ValueT & operator[](const KeyT & Key) const
		{
			ConstIterator Iter = Find(Key);
			if (Iter == End())
				throw EError::OutOfBound;
			return Iter->Value;
		}

		operator TView<PairT>() const
		{
			return BaseT::template operator TView<PairT>();
		}
	};
}


namespace std
{
	template<typename KeyT, typename ValueT>
	auto begin(XE::TLinearUnorderedMap<KeyT, ValueT> & Vector)
	{
		return Vector.Begin();
	}

	template<typename KeyT, typename ValueT>
	auto end(XE::TLinearUnorderedMap<KeyT, ValueT> & Vector)
	{
		return Vector.End();
	}

	template<typename KeyT, typename ValueT>
	auto begin(const XE::TLinearUnorderedMap<KeyT, ValueT> & Vector)
	{
		return Vector.Begin();
	}

	template<typename KeyT, typename ValueT>
	auto end(const XE::TLinearUnorderedMap<KeyT, ValueT> & Vector)
	{
		return Vector.End();
	}
}