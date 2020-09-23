#pragma once

#include "GraphicsInc.h"

namespace XE
{
	enum class EPathCommand
	{
		None = 0,
		Point,
		MoveTo,
		LineTo,
		Bezier3To,
		Bezier4To,
		ArcTo,

		FillAlternate,
		FillWinding,
		Close,
	};

	struct FPathPoint
	{
		EPathCommand Command = EPathCommand::None;
		PointF Point;
	};
}
