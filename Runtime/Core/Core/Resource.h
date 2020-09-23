#pragma once

#include "CoreInc.h"
#include "Object.h"

namespace XE
{
	//class CORE_API IResource : public IRefer
	//{
	//public:
	//	virtual FVariant GetKey() const = 0;
	//	virtual FType GetType() const = 0;
	//	virtual FStringView GetName() const = 0;

	//	TLinearUnorderedMap<FString, FVariant> Items;
	//};
	
	class CORE_API FResource : public FObject
	{
	public:
		FResource() = default;
		FResource(FType Type_) : Type(Type_) {}

		FVariant Key;
		FType Type;
		TLinearUnorderedMap<FString, const IDataProvider *> Items;

	public:
		static FProperty KeyProperty;
		friend CORE_API void Reflection(TNativeType<FResource> & NativeType);
	};
}
