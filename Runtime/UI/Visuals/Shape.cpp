#include "PCH.h"
#include "Shape.h"

namespace XE::UI
{
	FProperty UShape::FillProperty(Str("Fill"), &UShape::Fill);
	FProperty UShape::StrokeProperty(Str("Stroke"), &UShape::Stroke);
	FProperty UShape::StrokeThicknessProperty(Str("StrokeThickness"), &UShape::StrokeThickness);

	void Reflection(TNativeType<UShape> & NativeType)
	{
		NativeType.RegisterProperty(UShape::FillProperty);
		NativeType.RegisterProperty(UShape::StrokeProperty);
		NativeType.RegisterProperty(UShape::StrokeThicknessProperty);
	}
}
