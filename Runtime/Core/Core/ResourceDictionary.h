#pragma once

#include "CoreInc.h"
#include "Resource.h"

namespace XE
{
	class  CORE_API FResourceDictionary
	{
	public:
		FResourceDictionary() = default;
		FResourceDictionary(FStringView ResourceDirectoryFilePath);
		void MergeResources(FStringView ResourceDirectoryFilePath);

		const FVariant & operator[](FVariant Key) const
		{
			if (auto Iter = Resources.Find(Key); Iter != Resources.End())
				return Iter->Value;
			return FVariant::Empty;
		}
		
	public:
		TMap<FVariant, FVariant> Resources;

	private:
		TVector<TReferPtr<IDataProvider>> DataProviders;
	};
}