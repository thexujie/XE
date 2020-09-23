#pragma once

#include "UIInc.h"
#include "Control.h"

namespace XE::UI
{
	class UI_API UTitleBar : public UControl
	{
	public:
		UTitleBar() = default;

		EHitResult Hit(const PointF & Point);
		
	private:
		
	};
}
