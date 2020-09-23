#pragma once

#include "UIInc.h"
#include "Decorator.h"

namespace XE::UI
{
	class UI_API UBorder: public UDecorator
	{
	public:
		UBorder() = default;

	public:
		SizeF Measure(SizeF ConstrainedSize) const override;
		void Arrange(SizeF ArrangedSize) override;
		void OnRender(FGraphics & Graphics) const override;
		
	public:
		TReferPtr<FBrush> Background;
		TReferPtr<FBrush> BorderBrush;
		ThicknessF BorderThickness = { NanF, NanF, NanF, NanF };
		ThicknessF Padding = {};
		
	public:
		static FProperty BackgroundProperty;
		static FProperty BorderBrushProperty;
		static FProperty BorderThicknessProperty;
		static FProperty PaddingProperty;
		
		friend UI_API void Reflection(TNativeType<UBorder> & NativeType);
	};
}
