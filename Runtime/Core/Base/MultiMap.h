#pragma once

#include "Types.h"
#include "RBTree.h"
#include "Pair.h"

namespace XE
{
	template<typename KeyT, typename ValueT>
	struct TMultiMapItemTraitsT
	{
		template <typename KeyT2, typename ValueT2>
		static const KeyT & GetKey(const TPair<KeyT2, ValueT2> & Pair) { return Pair.Key; }

		template <typename KeyT2, typename ValueT2>
		static const ValueT & GetValue(const TPair<KeyT2, ValueT2> & Pair) { return Pair.Value; }

		template <typename KeyT2, typename ValueT2>
		static ValueT & GetValue(TPair<KeyT2, ValueT2> & Pair) { return Pair.Value; }
	};

	template<typename KeyT, typename ValueT, typename KeyComparerT = TLess<KeyT>>
	class TMultiMap : public TRBTree<TPair<KeyT, ValueT>, KeyComparerT, TMultiMapItemTraitsT<KeyT, ValueT>>
	{
	public:
		using BaseT = TRBTree<TPair<KeyT, ValueT>, KeyComparerT, TMultiMapItemTraitsT<KeyT, ValueT>>;
		using PairType = TPair<KeyT, ValueT>;
		using NodeType = typename BaseT::NodeType;
		using Iterator = typename BaseT::Iterator;
		using ConstIterator = typename BaseT::ConstIterator;

	public:
		TMultiMap() {}
		~TMultiMap()
		{
		}

		Iterator Insert(const KeyT & Key, const ValueT & Value)
		{
			return BaseT::Emplace(PairType(Key, Value));
		}

		TPair<Iterator, Iterator> Find(const KeyT & Key)
		{
			NodeType * Lower = BaseT::LowerBound(Key);
			if (Lower == BaseT::End().GetNode())
				return  { BaseT::End(), BaseT::End() };
			
			NodeType * Upper = BaseT::UpperBound(Key);
			return { Iterator(Lower) , Iterator(Upper) };
		}

		TPair<ConstIterator, ConstIterator> Find(const KeyT & Key) const
		{
			NodeType * Lower = BaseT::LowerBound(Key);
			if (Lower == BaseT::End().GetNode())
				return  { BaseT::End(), BaseT::End() };

			NodeType * Upper = BaseT::UpperBound(Key);
			return { Iterator(Lower), Iterator(Upper) };
		}
	};
}
