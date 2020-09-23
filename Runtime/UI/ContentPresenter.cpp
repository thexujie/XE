#include "PCH.h"
#include "ContentPresenter.h"
#include "ContentControl.h"
#include "Visuals/TextBlock.h"

namespace XE::UI
{
	void UContentPresenter::InitializeUI()
	{
		TReferPtr<UContentControl> ContentControl = TFindAncestor<UContentControl>();
		if (ContentControl)
		{
			Content = ContentControl->GetContent();

			if (Content.ValueType == EValueType::ReferPointer && Content.GetObjectType().IsChildOf(TypeOf<UElement>()))
			{
				Element = Content.Get<TReferPtr<UElement>>();
				AddChild(Element);
			}
			else if (Content.ValueType == EValueType::String)
			{
				TReferPtr<UTextBlock> TextBlock = MakeRefer<UTextBlock>();
				TextBlock->Text = Content.Get<FStringView>();
				AddChild(TextBlock);
				Element = TextBlock;
			}
			else {}
		}
		else
		{
			LogWarning(Str("UContentPresenter must contained by a ContentControl"));
		}

		UElement::InitializeUI();
	}

	SizeF UContentPresenter::Measure(SizeF ConstrainedSize) const
	{
		SizeF Size = {};
		for (const TReferPtr<UElement> & Child : Children)
		{
			Child->LayoutMeasure(ConstrainedSize);
			Size.Width = Max(Size.Width, Child->DesiredSize.Width);
			Size.Height = Max(Size.Height, Child->DesiredSize.Height);
		}
		return Size;
	}

	void UContentPresenter::Arrange(SizeF ArrangedSize)
	{
		for (const TReferPtr<UElement> & Child : Children)
		{
			Child->LayoutArrange(RectF(PointF(), ArrangedSize));
		}
	}
}
