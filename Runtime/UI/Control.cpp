#include "PCH.h"
#include "Control.h"


namespace XE::UI
{
	FProperty UControl::TemplateProperty(Str("Template"), &UControl::Template);
	FProperty UControl::ForegroundProperty(Str("Foreground"), &UControl::Foreground);
	FProperty UControl::BackgroundProperty(Str("Background"), &UControl::Background);
	
	void UControl::SetForeground(TReferPtr<FBrush> Brush)
	{
		Foreground = Brush;
	}

	void UControl::SetBackground(TReferPtr<FBrush> Brush)
	{
		Background = Brush;
	}

	TReferPtr<FBrush> UControl::GetForeground()
	{
		return Foreground;
	}

	TReferPtr<FBrush> UControl::GetBackground()
	{
		return Background;
	}

	void UControl::ApplyStyle()
	{
		UElement::ApplyStyle();
		ApplyTemplate();
	}
	
	void UControl::ApplyTemplate()
	{
		if (!Template)
			return;

		auto Element = Template->LoadContent();
		AddChild(Element);
	}

	void Reflection(TNativeType<UControl> & NativeType)
	{
		NativeType.RegisterProperty(UControl::TemplateProperty);
		NativeType.RegisterProperty(UControl::ForegroundProperty);
		NativeType.RegisterProperty(UControl::BackgroundProperty);
	}
}
