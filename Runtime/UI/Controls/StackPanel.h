#pragma once

#include "UIInc.h"
#include "Panel.h"

namespace XE::UI
{
	class UI_API UStackPanel : public UPanel
	{
	public:
		UStackPanel() = default;

		SizeF Measure(SizeF ConstrainedSize) const override;
		void Arrange(SizeF ArrangedSize) override;
		
		void OnRender(FGraphics & Graphics) const;

	public:
		EOrientation Orientation = EOrientation::Horizontal;

	public:
		static FProperty OrientationProperty;
		friend UI_API void Reflection(TNativeType<UStackPanel> & NativeType);
	};

	class UI_API UHBox : public UStackPanel
	{
	public:
		UHBox() { Orientation = EOrientation::Horizontal; }
	};

	class UI_API UVBox : public UStackPanel
	{
	public:
		UVBox() { Orientation = EOrientation::Vertical; }
	};
}
