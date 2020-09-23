#pragma once

#include "UIInc.h"
#include "Element.h"
#include "ControlTemplate.h"
#include "Style.h"

namespace XE::UI
{
	class UI_API UControl : public UElement
	{
	public:
		UControl() = default;

		void SetControlTemplate(TReferPtr<FControlTemplate> Template_) { Template = Template_; }

	public:
		TReferPtr<FBrush> GetBackground();
		TReferPtr<FBrush> GetForeground();
		void SetForeground(TReferPtr<FBrush> Brush);
		void SetBackground(TReferPtr<FBrush> Brush);

	public:
		virtual EHitResult Hit(const PointF & Point) { return EHitResult::Client; }

	public:
		void ApplyStyle() override;

	public:
		virtual void ApplyTemplate();
		
	protected:
		
		TReferPtr<FControlTemplate> Template;

		TReferPtr<FBrush> Foreground;
		TReferPtr<FBrush> Background;

		EHorizontalAlignment  HorizontalAlignment = EHorizontalAlignment::Left;
		EVerticalAlignment VerticalAlignment = EVerticalAlignment::Top;

	public:
		static FProperty TemplateProperty;
		static FProperty ForegroundProperty;
		static FProperty BackgroundProperty;
		friend UI_API void Reflection(TNativeType<UControl> & NativeType);
	};
}
