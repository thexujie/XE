#pragma once

#include "Types.h"
#include "RBTree.h"
#include "Pair.h"

namespace XE
{
	template<typename KeyT>
	struct TSetItemTraitsT
	{
		template <typename KeyT2>
		static const KeyT & GetKey(const KeyT2 & Key) { return Key; }

		template <typename KeyT2>
		static const KeyT & GetValue(const KeyT2 & Key) { return Key; }

		template <typename KeyT2>
		static KeyT & GetValue(const KeyT2 & Key) { return Key; }
	};
	
	template<typename KeyT, typename KeyComparerT = TLess<KeyT>>
	class TSet : public TRBTree<KeyT, KeyComparerT, TSetItemTraitsT<KeyT>>
	{
	public:
		using BaseT = TRBTree<KeyT, KeyComparerT, TSetItemTraitsT<KeyT>>;
		using Iterator = typename BaseT::Iterator;
		using ConstIterator = typename BaseT::ConstIterator;

	public:
		TSet() {}
		~TSet()
		{
		}

		TPair<Iterator, bool> Insert(const KeyT & Key)
		{
			return BaseT::Insert(Key);
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
	};
}
