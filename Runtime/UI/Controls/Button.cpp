#include "PCH.h"
#include "Button.h"

namespace XE::UI
{
	SizeF UButton::Measure(SizeF ConstrainedSize) const
	{
		return UContentControl::Measure(ConstrainedSize);
	}
	
	void UButton::Arrange(SizeF ArrangedSize)
	{
		return UContentControl::Arrange(ArrangedSize);
	}
	
	void UButton::OnRender(FGraphics & Graphics) const
	{
		//Graphics.FillRectangle(RenderRect, Brushs::LightGray);
	}
}
