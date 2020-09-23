#pragma once

#include "Hash.h"
#include "Types.h"
#include "Map.h"

namespace XE
{
	template<typename KeyT, typename ValueT, typename KeyHashT = THash<KeyT>>
	class THashMap : protected TMap<uintx_t, ValueT>
	{
		using BaseT = TMap<uintx_t, ValueT>;
		using Iterator = typename BaseT::Iterator;
		using ConstIterator = typename BaseT::ConstIterator;
		
	public:
		Iterator InsertOrAssign(const KeyT & Key, const ValueT & Value)
		{
			return BaseT::InsertOrAssign(KeyHashT()(Key), Value).Key;
		}

		const ValueT & operator[](const KeyT & Key) const
		{
			ConstIterator Iter = BaseT::Find(KeyHashT()(Key));
			if (Iter == BaseT::End())
				throw EError::NotFound;

			return Iter->Value;
		}

		Iterator Find(const KeyT & Key)
		{
			return BaseT::Find(KeyHashT()(Key));
		}
		
		ConstIterator Find(const KeyT & Key) const
		{
			return BaseT::Find(KeyHashT()(Key));
		}

		Iterator Begin() const
		{
			return BaseT::Begin();
		}

		ConstIterator End() const
		{
			return BaseT::End();
		}
	};
}