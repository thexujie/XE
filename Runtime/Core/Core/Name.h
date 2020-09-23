#pragma once

#include "CoreInc.h"

namespace XE
{
	class CORE_API FNameSpace
	{
	public:
		FNameSpace() = default;
		FNameSpace(FStringView Name_) : Name(Name_) {}
		FNameSpace(FStringView Name_, const FNameSpace & Parent_) : Parent(Parent_), Name(Name_) { Parent_.Children.InsertOrAssign(Name_, *this); }
		FNameSpace(const FNameSpace &) = delete;
		FNameSpace & operator = (const FNameSpace &) = delete;
		
		auto operator <=> (const FNameSpace &) const = default;
		bool operator == (const FNameSpace & Another) const
		{
			return Name == Another.Name && (&Parent == &Another.Parent);
		}
		
		const FNameSpace & Parent = *static_cast<const FNameSpace*>(nullptr);
		FString Name;
		mutable TMap<FString, TReference<const FNameSpace>> Children;
	};

	namespace NameSpaces
	{
		CORE_API extern FNameSpace Unkown;
		CORE_API extern FNameSpace Root;
		CORE_API extern FNameSpace X;
		CORE_API extern FNameSpace Xmlns;
	}

	class CORE_API FName
	{
	public:
		FName() = default;
		FName(FStringView FullName) : Name(ParseName(FullName)), NameSpace(ParseNameSpace(FullName)) {}

		FString Name;
		const FNameSpace & NameSpace = NameSpaces::Unkown;

	public:
		static const FNameSpace & ParseNameSpace(FStringView FullName);
		static FStringView ParseName(FStringView FullName);
	};
}