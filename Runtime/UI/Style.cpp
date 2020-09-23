#include "PCH.h"
#include "Style.h"

namespace XE::UI
{
	FProperty FStyle::TargetTypeProperty(Str("TargetType"), &FStyle::TargetType);

	void Reflection(TNativeType<FStyle> & NativeType)
	{
		NativeType.RegisterProperty(FStyle::TargetTypeProperty);
	}
}

namespace XE
{
	using namespace UI;
	
	FVariant TDataConverter<FStyle>::Convert(const IDataProvider * DataProvider) const
	{
		TReferPtr<FStyle> Style = MakeRefer<FStyle>();
		for (const IDataProvider * PropertyDataProvider = DataProvider->GetFirstAttribute();
			PropertyDataProvider != nullptr;
			PropertyDataProvider = PropertyDataProvider->GetSibling())
		{
			FName PropertyName(PropertyDataProvider->GetName());
			if (!PropertyName.NameSpace.Name.IsEmpty())
				continue;
			
			if (PropertyName.Name.IsEmpty())
			{
				LogWarning(Str("Setter need a property attribute"));
				continue;
			}

			FProperty Property = TypeOf<FStyle>().GetProperty(PropertyName.Name);
			if (!Property)
			{
				LogWarning(Str("Unrecgenized property {} in {}"), PropertyName.Name, Style->TargetType.GetName());
				continue;
			}

			FVariant PropertyValue = TypeOf<FVariant>().CreateValue(PropertyDataProvider);
			Property.SetValue(Style.Get(), PropertyValue);
		}

		for (const IDataProvider * ChildDataProvider = DataProvider->GetFirstChild();
			ChildDataProvider != nullptr;
			ChildDataProvider = ChildDataProvider->GetSibling())
		{
			FStringView ChildDataName = ChildDataProvider->GetName();
			if (ChildDataName != Str("Setter"))
			{
				LogWarning(Str("Setter nodes needed for style"));
				continue;
			}

			FStringView PropertyName = FStringView::Empty;
			const IDataProvider * PropertyValueDataProvider = nullptr;
			for (const IDataProvider * SetterPropertyDataProvider = ChildDataProvider->GetFirstAttribute();
				SetterPropertyDataProvider != nullptr;
				SetterPropertyDataProvider = SetterPropertyDataProvider->GetSibling())
			{
				if (SetterPropertyDataProvider->GetName() == Str("Name"))
				{
					PropertyName = SetterPropertyDataProvider->GetValue();
				}
				else if (SetterPropertyDataProvider->GetName() == Str("Value"))
				{
					PropertyValueDataProvider = SetterPropertyDataProvider;
				}
			}

			if (PropertyName == FStringView::Empty)
			{
				LogWarning(Str("Setter need a property attribute"));
				continue;
			}

			if (PropertyValueDataProvider)
			{
				FProperty Property = Style->TargetType.GetProperty(PropertyName);
				if (!Property)
				{
					LogWarning(Str("Unrecgenized property {} in {}"), PropertyName, Style->TargetType.GetName());
					continue;
				}

				FVariant Content = Property.GetType().CreateValue(PropertyValueDataProvider);
				Style->Properties[Property] = Content;
			}
			else if (const IDataProvider * SetterContentDataProvider = ChildDataProvider->GetFirstChild())
			{
				FProperty Property = Style->TargetType.GetProperty(PropertyName);
				if (!Property)
				{
					LogWarning(Str("Unrecgenized property {} in {}"), PropertyName, Style->TargetType.GetName());
					continue;
				}

				Style->Properties[Property] = TypeOf<FVariant>().CreateValue(SetterContentDataProvider);
			}
			else
			{
				LogWarning(Str("Setter need one content child node"));
			}
		}
		return Style;
	}
}
