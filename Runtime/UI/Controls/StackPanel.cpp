#include "PCH.h"
#include "StackPanel.h"

namespace XE::UI
{
	FProperty UStackPanel::OrientationProperty(Str("Orientation"), &UStackPanel::Orientation);
	void Reflection(TNativeType<UStackPanel> & NativeType)
	{
		NativeType.RegisterProperty(UStackPanel::OrientationProperty);
	}
	
	void UStackPanel::OnRender(FGraphics & Graphics) const
	{
		//Graphics.FillRectangle({0, 0, RenderRect.Width, RenderRect .Height}, Brushs::GhostWhite);
	}

	SizeF UStackPanel::Measure(SizeF ConstrainedSize) const
	{
		SizeF Size = {};
		for (const TReferPtr<UElement> & Child : Children)
		{
			Child->LayoutMeasure(ConstrainedSize);
			if (Orientation == EOrientation::Horizontal)
			{
				Size.Width += Child->DesiredSize.Width;
				Size.Height = Max(Size.Height, Child->DesiredSize.Height);
			}
			else
			{
				Size.Width = Max(Size.Width, Child->DesiredSize.Width);
				Size.Height += Child->DesiredSize.Height;
			}
		}
		return Size;
	}
	
	void UStackPanel::Arrange(SizeF ArrangedSize)
	{
		PointF ChildPosition(0.0f, 0.0f);
		for (const TReferPtr<UElement> & Child : Children)
		{
			Child->LayoutArrange(RectF(ChildPosition, Child->DesiredSize));
			if (Orientation == EOrientation::Horizontal)
				ChildPosition.X += Child->DesiredSize.Width;
			else
				ChildPosition.Y += Child->DesiredSize.Height;
		}
	}
}