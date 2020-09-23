#include "PCH.h"
#include "ControlTemplate.h"
#include "Control.h"

namespace XE::UI
{
	FProperty FControlTemplate::TargetTypeProperty(Str("TargetType"), &FControlTemplate::TargetType);

	void Reflection(TNativeType<FControlTemplate> & NativeType)
	{
		NativeType.RegisterProperty(FControlTemplate::TargetTypeProperty);
	}
	
	TReferPtr<UElement> FControlTemplate::LoadContent()
	{
		return TypeOf<UControl>().CreateValue(DataProvider).Get<TReferPtr<UElement>>();
	}
}

namespace XE
{
	FVariant TDataConverter<UI::FControlTemplate>::Convert(const IDataProvider * DataProvider) const
	{
		TReferPtr<UI::FControlTemplate> ControlTemplate = MakeRefer<UI::FControlTemplate>();
		ControlTemplate->DataProvider = DataProvider->GetFirstChild();

		for (const IDataProvider * PropertyDataProvider = DataProvider->GetFirstAttribute();
			PropertyDataProvider != nullptr;
			PropertyDataProvider = PropertyDataProvider->GetSibling())
		{
			FName PropertyName(PropertyDataProvider->GetName());
			if (!PropertyName.NameSpace.Name.IsEmpty())
				continue;

			FProperty Property = TypeOf<UI::FControlTemplate>().GetProperty(PropertyName.Name);
			if (!Property)
			{
				LogWarning(Str("Unrecgenized property {} in {}"), PropertyName.Name, ControlTemplate->TargetType.GetName());
				continue;
			}

			FVariant PropertyValue = TypeOf<FVariant>().CreateValue(PropertyDataProvider);
			Property.SetValue(ControlTemplate.Get(), PropertyValue);
		}
		
		return ControlTemplate;
	}
}
