#include "PCH.h"
#include "Object.h"

namespace XE
{
	void FChangable::SetProperty(FProperty Property, FVariant PropertyValue)
	{
		OnPropertyChanging(Property, Properties[Property], PropertyValue);
		Properties[Property] = PropertyValue;
		OnPropertyChanged(Property, Properties[Property], PropertyValue);
		PropertyChanged.Excute(Property);
	}
}
