#include "PCH.h"
#include "Border.h"


namespace XE::UI
{
	FProperty UBorder::BackgroundProperty(Str("Background"), &UBorder::Background);
	FProperty UBorder::BorderBrushProperty(Str("BorderBrush"), &UBorder::BorderBrush);
	FProperty UBorder::BorderThicknessProperty(Str("BorderThickness"), &UBorder::BorderThickness);
	FProperty UBorder::PaddingProperty(Str("Padding"), &UBorder::Padding);

	void Reflection(TNativeType<UBorder> & NativeType)
	{
		NativeType.RegisterProperty(UBorder::BackgroundProperty);
		NativeType.RegisterProperty(UBorder::BorderBrushProperty);
		NativeType.RegisterProperty(UBorder::BorderThicknessProperty);
		NativeType.RegisterProperty(UBorder::PaddingProperty);
	}
	
	SizeF UBorder::Measure(SizeF ConstrainedSize) const
	{
		SizeF BorderSize = { BorderThickness.Left + BorderThickness.Right, BorderThickness.Top + BorderThickness.Bottom };
		SizeF PaddingSize = { Padding.Left + Padding.Right, Padding.Top + Padding.Bottom };
		if (!Child)
			return BorderSize + PaddingSize;

		Child->LayoutMeasure(ConstrainedSize - BorderSize);
		return BorderSize + PaddingSize + Child->DesiredSize;
	}
	
	void UBorder::Arrange(SizeF ArrangedSize)
	{
		if (Child)
		{
			PointF ChildPosition = { BorderThickness.Left + Padding.Left, BorderThickness.Top + Padding.Top };
			SizeF ChildSize = ArrangedSize - BorderThickness.ThicknessSize() - Padding.ThicknessSize();
			Child->LayoutArrange(RectF(ChildPosition, ChildSize));
		}
	}

	void UBorder::OnRender(FGraphics & Graphics) const
	{
		if (Background)
			Graphics.FillRectangle(RectF(PointF(), RenderRect.Size), *Background);
		Graphics.DrawRectangle(RectF(PointF(), RenderRect.Size), *BorderBrush, BorderThickness.Left);
	}
}

