#include "PCH.h"
#include "ContentControl.h"


namespace XE::UI
{
	FProperty UContentControl::ContentProperty(Str("Content"), &UContentControl::Content);

	void Reflection(TNativeType<UContentControl> & NativeType)
	{
		NativeType.RegisterProperty(UContentControl::ContentProperty);
	}

	FStringView UContentControl::ToString(FStringView Formal) const
	{
		return Format(Str("{} {{{}}}, |{}|"), GetObjectType().GetShortName(), RenderRect, Content);
	}
	
	SizeF UContentControl::Measure(SizeF ConstrainedSize) const
	{
		SizeF MeasuredSize = {};
		for (const TReferPtr<UElement> & Child : Children)
		{
			Child->LayoutMeasure(ConstrainedSize);
			MeasuredSize.Width = Max(MeasuredSize.Width, Child->DesiredSize.Width);
			MeasuredSize.Height = Max(MeasuredSize.Height, Child->DesiredSize.Height);
		}
		return MeasuredSize;
	}
	
	void UContentControl::Arrange(SizeF ArrangedSize)
	{
		SizeF Size = {};
		for (const TReferPtr<UElement> & Child : Children)
		{
			Child->LayoutArrange(RectF(PointF(), ArrangedSize));
			Size.Width = Max(Size.Width, Child->RenderRect.Width);
			Size.Height = Max(Size.Height, Child->RenderRect.Height);
		}
	}
}

