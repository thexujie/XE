#include "PCH.h"
#include "Type.h"

namespace XE
{
	FType FType::Unkown;
	FProperty FProperty::Empty;

	const FType & IObject::GetObjectType() const
	{
		return TypeBy(typeid(*this).name());
	}

	FStringView IObject::ToString(FStringView Formal) const
	{
		return Format(Str("{{{}}} Object"), GetObjectType().GetName());
	}
	
	static TVector<TReferPtr<IPropertyAccessor>> PropertyAccessors;
	FProperty::FProperty(TReferPtr<IPropertyAccessor> PropertyAccessor_) : PropertyAccessor(PropertyAccessor_.Get())
	{
		PropertyAccessors.Add(PropertyAccessor_);
	}

	static TVector<TReferPtr<IMethodAccessor>> MethodAccessors;
	FMethod::FMethod(TReferPtr<IMethodAccessor> MethodAccessor_) : MethodAccessor(MethodAccessor_.Get())
	{
		MethodAccessors.Add(MethodAccessor_);
	}

	FStringView FType::GetNormalName() const
	{
		FStringView Name = GetName();
		if (Name.StartsWith(Str("class ")))
			return Name.Slice(6);
		else if (Name.StartsWith(Str("struct ")))
			return Name.Slice(7);
		else
			return Name;
	}
	
	FStringView FType::GetShortName() const
	{
		FStringView Name = GetName();
		if (size_t Index = Name.FindLastOf(Str(" :")); Index != NullIndex)
			Name = Name.Slice(Index + 1);
		if (Name[0] == Str('U') || Name[0] == Str('F'))
			Name = Name.Slice(1);
		return Name;
	}

	bool INativeType::IsParentOf(const INativeType * Another) const
	{
		if (!Another)
			return false;

		const INativeType * NativeType = GetFirstChild();
		while (NativeType)
		{
			if (NativeType == Another || Another->IsParentOf(NativeType))
				return true;
			NativeType = NativeType->GetNextSibling();
		}
		return false;
	}

	bool INativeType::IsChildOf(const INativeType * Another) const
	{
		if (!Another)
			return false;
		
		const INativeType * NativeType = Another->GetFirstChild();
		while (NativeType)
		{
			if (NativeType == this || IsChildOf(NativeType))
				return true;
			NativeType = NativeType->GetNextSibling();
		}
		return false;
	}
}
