#pragma once

#include "Types.h"
#include "RBTree.h"
#include "Pair.h"

namespace XE
{
	template<typename KeyT, typename ValueT>
	struct TMapItemTraitsT
	{
		template <typename KeyT2, typename ValueT2>
		static const KeyT & GetKey(const TPair<KeyT2, ValueT2> & Pair) { return Pair.Key; }

		template <typename KeyT2, typename ValueT2>
		static const ValueT & GetValue(const TPair<KeyT2, ValueT2> & Pair) { return Pair.Value; }

		template <typename KeyT2, typename ValueT2>
		static ValueT & GetValue(TPair<KeyT2, ValueT2> & Pair) { return Pair.Value; }
	};
	
	template<typename KeyT, typename ValueT, typename KeyComparerT = TLess<KeyT>>
	class TMap : public TRBTree<TPair<KeyT, ValueT>, KeyComparerT, TMapItemTraitsT<KeyT, ValueT>>
	{
	public:
		using BaseT = TRBTree<TPair<KeyT, ValueT>, KeyComparerT, TMapItemTraitsT<KeyT, ValueT>>;
		using PairType = TPair<KeyT, ValueT>;
		using Iterator = typename BaseT::Iterator;
		using ConstIterator = typename BaseT::ConstIterator;

	public:
		TMap() {}
		~TMap()
		{
		}

		template<typename ValueT2>
		TPair<Iterator, bool> InsertOrAssign(const KeyT & Key, const ValueT2 & Value)
		{
			return BaseT::InsertOrAssign(PairType(Key, Value));
		}

		template<typename ValueT2>
		TPair<Iterator, bool> InsertOrAssign(KeyT && Key, ValueT2 && Value)
		{
			return BaseT::InsertOrAssign(PairType(Key, Value));
		}
		
		TPair<Iterator, bool> Insert(const PairType & Pair)
		{
			return BaseT::InsertOrAssign(Pair);
		}

		Iterator Find(const KeyT & Key)
		{
			return BaseT::template Find<KeyT>(Key);
		}

		ConstIterator Find(const KeyT & Key) const
		{
			return BaseT::template Find<KeyT>(Key);
		}

		bool Contains(const KeyT & Key) const
		{
			return Find(Key) != BaseT::End();
		}
		
		ValueT & operator[](const KeyT & Key)
		{
			TPair<Iterator, bool> Result = BaseT::Access(Key);
			return (Result.Key)->Value;
		}

		const ValueT & operator[](const KeyT & Key) const
		{
			ConstIterator Iter = Find(Key);
			if (Iter == BaseT::End())
				throw EError::NotFound;
			return Iter->Value;
		}
	};
}
