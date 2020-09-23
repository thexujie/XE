#include "PCH.h"

#include "Element.h"
#include "Control.h"
#include "ContentControl.h"
#include "Panel.h"
#include "Decorator.h"

namespace XE
{
	using namespace UI;
	
	FVariant TDataConverter<UElement>::Convert(const IDataProvider * DataProvider) const
	{
		FStringView ControlTypeName = DataProvider->GetName();
		FType ControlType = TypeBy(ControlTypeName);
		if (!ControlType.DefaultConvertableTo(TypeOf<UElement>()))
		{
			LogWarning(Str("Invalid value type, but input {}"), ControlType.GetName());
			return nullptr;
		}

		TReferPtr<UElement> Element = ControlType.CreateValue().Get<TReferPtr<UElement>>();
		for (const IDataProvider * PropertyDataProvider = DataProvider->GetFirstAttribute();
			PropertyDataProvider != nullptr;
			PropertyDataProvider = PropertyDataProvider->GetSibling())
		{
			FProperty Property = ControlType.GetProperty(PropertyDataProvider->GetName());
			if (!Property)
			{
				LogInfo(Str("Can not find property accessor \"{}\" in \"{}\""), PropertyDataProvider->GetName(), ControlType.GetName());
				continue;
			}

			FType PropertyType = Property.GetType();
			if (!PropertyType)
			{
				LogInfo(Str("Invalid property type of {} in {}"), PropertyDataProvider->GetName(), ControlType.GetName());
				continue;
			}

			Element->SetProperty(Property, PropertyType.CreateValue(PropertyDataProvider));
		}

		if (DataProvider->GetFirstChild() == nullptr)
			return Element;

		// Container
		if (ControlType.IsChildOf(TypeOf<UContentControl>()))
		{
			if (DataProvider->GetFirstChild()->GetSibling())
			{
				LogInfo(Str("Container {} is container, one child only"), ControlType.GetName());
				return Element;
			}

			const IDataProvider * ContentDataProvider = DataProvider->GetFirstChild();

			FStringView ContentValueTypeName = ContentDataProvider->GetName();
			FType ContentValueType = TypeBy(ContentValueTypeName);

			TReferPtr<UContentControl> Container = StaticPointerCast<UContentControl>(Element);
			Container->SetContent(ContentValueType.CreateValue(ContentDataProvider));
			return FVariant(Element);
		}
		else if (ControlType.IsChildOf(TypeOf<UPanel>()))
		{
			TReferPtr<UPanel> Panel = StaticPointerCast<UPanel>(Element);
			for (const IDataProvider * ChildDataProvider = DataProvider->GetFirstChild();
				ChildDataProvider != nullptr;
				ChildDataProvider = ChildDataProvider->GetSibling())
			{
				FStringView ChildValueTypeName = ChildDataProvider->GetName();
				FType ChildValueType = TypeBy(ChildValueTypeName);
				FVariant ChildValue = ChildValueType.CreateValue(ChildDataProvider);

				TReferPtr<UControl> Child = ChildValue.Get<TReferPtr<UControl>>();
				Panel->AddPanelControl(Child);
			}
			return Panel;
		}
		else if (ControlType.IsChildOf(TypeOf<UDecorator>()))
		{
			if (DataProvider->GetFirstChild()->GetSibling())
			{
				LogInfo(Str("{} is decorator, one child only"), ControlType.GetName());
				return Element;
			}

			const IDataProvider * ChildDataProvider = DataProvider->GetFirstChild();

			FStringView ChildValueTypeName = ChildDataProvider->GetName();
			FType ChildValueType = TypeBy(ChildValueTypeName);
			if (!ChildValueType.IsChildOf(TypeOf<UElement>()))
			{
				LogInfo(Str("{} is decorator, {} is an unknown or not UElement child."), ControlType.GetName(), ChildValueTypeName);
				return Element;
			}

			TReferPtr<UDecorator> Decorator = StaticPointerCast<UDecorator>(Element);
			Decorator->SetChild(ChildValueType.CreateValue(ChildDataProvider).Get<TReferPtr<UElement>>());
			return Element;
		}
		else
		{
			LogWarning(Str("Unrecgnized type {} in UIFactory"), ControlType.GetName());
			return Element;
		}
	}
}
