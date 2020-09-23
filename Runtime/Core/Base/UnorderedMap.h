#pragma once

#include "Types.h"
#include "Pair.h"

namespace XE
{
	template<typename KeyT, typename ValueT, typename KeyComparerT = TEqual<KeyT>>
	class TUnorderedMap : public TVector<TPair<KeyT, ValueT>>
	{
		using PairType = TPair<KeyT, ValueT>;
		using ContainerType = TVector<TPair<KeyT, ValueT>>;

	public:
		TUnorderedMap() = default;
		TUnorderedMap(const TUnorderedMap &) = default;
		TUnorderedMap(TUnorderedMap &&) = default;

		PairType * InsertOrAssign(const KeyT & Key, const ValueT & Value)
		{
			PairType * PairFound = ContainerType::Find([&Key](auto & PairIn) { return KeyComparerT()(PairIn.Key, Key); });
			if (PairFound)
			{
				PairFound->Value = Value;
				return PairFound;
			}
			else
			{
				return &ContainerType::Add(MakePair<KeyT, ValueT>(Key, Value));
			}
		}

		PairType * InsertOrAssign(const PairType & Pair)
		{
			PairType * PairFound = ContainerType::Find([&Pair](auto & PairIn) { return KeyComparerT()(PairIn.Key, Pair.Key); });
			if (PairFound)
			{
				PairFound->Value = Pair.Value;
				return PairFound;
			}
			else
			{
				return &ContainerType::Add(Pair);
			}
		}

		ValueT & operator[](const KeyT & Key)
		{
			return InsertOrAssign(Key, ValueT())->Value;
		}
	};
}