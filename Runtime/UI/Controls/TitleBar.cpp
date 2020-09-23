#include "PCH.h"
#include "TitleBar.h"


namespace XE::UI
{
	EHitResult UTitleBar::Hit(const PointF & Point)
	{
		return EHitResult::Caption;
	}
}
