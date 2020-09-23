#pragma once

#include "UIInc.h"
#include "ContentControl.h"

namespace XE::UI
{
	class UI_API UButton : public UContentControl
	{
	public:
		UButton() = default;

		void OnRender(FGraphics & Graphics) const override;
		
	public:
		SizeF Measure(SizeF ConstrainedSize) const override;
		void Arrange(SizeF ArrangedSize) override;
	};
}
