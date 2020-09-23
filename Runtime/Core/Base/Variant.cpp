#include "PCH.h"
#include "Variant.h"

namespace XE
{
	FVariant FVariant::Empty;

	class FBindingExpresion
	{
	public:
		FBindingExpresion() = default;
		FBindingExpresion(FStringView Expresion_)
		{
			LoadExpresion(Expresion_);
		}

		void LoadExpresion(FStringView Expresion_)
		{
			Expresion = Expresion_.Slice(1, Expresion_.Size - 2);
			size_t SpaceIndex = Expresion.FindFirst(Str(' '));
			BindingGroup = Expresion.Slice(0, SpaceIndex);
			BindingParam = Expresion.Slice(SpaceIndex + 1);
		}


	public:
		FString Expresion;
		FStringView BindingGroup;
		FStringView BindingParam;
	};

	enum class EBindingExpresionType
	{

	};

	bool IsBindingExpression(FStringView Text)
	{
		Text = Strings::Trim(Text);
		return Text.StartsWith(Str('{')) && Text.EndsWith(Str('}'));
	}

	FVariant TDataConverter<FVariant>::Convert(const IDataProvider * DataProvider) const
	{
		if (DataProvider->GetFirstChild())
		{
			FStringView DataTypeName = DataProvider->GetName();
			FType DataType = TypeBy(DataTypeName);
			if (!DataType)
			{
				LogWarning(Str("Invalid data type {}"), DataTypeName);
				return nullptr;
			}

			return DataType.CreateValue(DataProvider);
		}
		else
		{
			FStringView PropertyValue = DataProvider->GetValue();
			if (IsBindingExpression(PropertyValue))
			{
				FBindingExpresion BindingExpresion(PropertyValue);
				if (BindingExpresion.BindingGroup == Str("x:Type"))
				{
					return TypeBy(BindingExpresion.BindingParam);
				}
			}
			return PropertyValue;
		}
	}
}
