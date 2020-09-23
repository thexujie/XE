#include "PCH.h"
#include "Decorator.h"

namespace XE::UI
{
	FProperty UDecorator::ChildProperty(Str("Child"), &UDecorator::Child);
	
	void Reflection(TNativeType<UDecorator> & NativeType)
	{
		NativeType.RegisterProperty(UDecorator::ChildProperty);
	}

	void UDecorator::InitializeUI()
	{
		AddChild(Child);
		UElement::InitializeUI();
	}
}
