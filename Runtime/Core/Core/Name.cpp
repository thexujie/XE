#include "PCH.h"
#include "Name.h"

namespace XE
{
	namespace NameSpaces
	{
		FNameSpace Unkown;
		FNameSpace Root(Str("<root>"));
		FNameSpace X(Str("x"), Root);
		FNameSpace Xmlns(Str("xmlns"), Root);
	}

	const FNameSpace & FName::ParseNameSpace(FStringView FullName)
	{
		TVector<FStringView> NameParts = Strings::Split(FullName, Str(':'));
		if (NameParts.Size == 1)
			return NameSpaces::Unkown;

		TReference<const FNameSpace> Current = NameSpaces::Root;
		for(size_t Index = 0; Index < NameParts.Size - 1; ++Index)
		{
			auto Iter = FindIf(Current.Value.Children, [&NameParts, &Index](const TPair<FString, TReference<const FNameSpace>> & Item) { return Item.Key == NameParts[Index]; });
			if (Iter == Current.Value.Children.End())
				break;
			Current = TReference<const FNameSpace>(Iter->Value);
		}
		return Current.Value;
	}
	
	FStringView FName::ParseName(FStringView FullName)
	{
		if (size_t Index = FullName.FindLast(Str(':')); Index != NullIndex)
			return FullName.Slice(Index);
		return FullName;
	}
}