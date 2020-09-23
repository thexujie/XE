#include "PCH.h"
#include "Form.h"


namespace XE::UI
{
	FProperty UForm::TitleProperty(Str("Title"), &UForm::Title);

	void Reflection(TNativeType<UForm> & NativeType)
	{
		NativeType.RegisterProperty(UForm::TitleProperty);
	}

	void UForm::OnRender(FGraphics & Graphics) const
	{
		if (Background)
			Graphics.FillRectangle(RectF(PointF(), RenderRect.Size), *Background);
		Graphics.DrawString(Str("ABCD·çÎ¶"), PointF(100, 100), FTextFormat(), *Foreground);
	}

	void UForm::SelectElement(TReferPtr<UElement> Element)
	{
		if (Element == SelectedElement)
			return;

		if (SelectedElement)
			SelectedElement->Selected = false;
		
		SelectedElement = Element;
		
		if (SelectedElement)
			SelectedElement->Selected = true;
	}
}
