#pragma once

#include "UIInc.h"
#include "Element.h"

namespace XE::UI
{
	class UI_API UShape : public UElement
	{
	public:
		UShape() = default;

	protected:
		TReferPtr<FBrush> Fill;
		TReferPtr<FBrush> Stroke;
		float32_t StrokeThickness = 0.0f;

	public:
		static FProperty FillProperty;
		static FProperty StrokeProperty;
		static FProperty StrokeThicknessProperty;
		friend UI_API void Reflection(TNativeType<UShape> & NativeType);
	};
}