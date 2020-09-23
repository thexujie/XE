#pragma once

#include "UIInc.h"
#include "Control.h"
#include "ContentPresenter.h"

namespace XE::UI
{
	class UI_API UContentControl: public UControl
	{
	public:
		UContentControl() = default;
		FStringView ToString(FStringView Formal = FStringView::Empty) const override;
		
	public:
		virtual EHitResult Hit(const PointF & Point) { return EHitResult::Client; }

	public:
		void SetContent(FVariant Content_)
		{
			Content = Content_;
		}

		FVariant GetContent() const
		{
			return Content;
		}
		

	public:
		SizeF Measure(SizeF ConstrainedSize) const override;
		void Arrange(SizeF ArrangedSize) override;
		
	public:
		FVariant Content;

		TReferPtr<UContentPresenter> ContentPresenter;
		
	public:
		static FProperty ContentProperty;
		friend UI_API void Reflection(TNativeType<UContentControl> & NativeType);
	};
}
