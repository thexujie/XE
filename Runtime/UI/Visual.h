#pragma once

#include "UIInc.h"
#include "Property.h"

namespace XE::UI
{
	class UI_API UVisual : public FUIObject
	{
	public:
		UVisual() = default;
		FStringView ToString(FStringView Formal = FStringView::Empty) const override;

	public:
		RectF RenderRect = {};
	};
}

