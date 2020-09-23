#pragma once

#include "UIInc.h"
#include "Control.h"

namespace XE::UI
{
	enum class ELayoutDirection
	{
		LeftToRight = 0,
		TopToBottom,
	};
	
	class UI_API ULayout : public UControl
	{
	public:
		ULayout() = default;


	protected:
		ELayoutDirection LayoutDirection = ELayoutDirection::LeftToRight;
	};
}
