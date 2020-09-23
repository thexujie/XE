#pragma once

#include "GraphicsInc.h"

namespace XE
{
	struct STextLayoutMetrics
	{
		SizeF Size = {};
		float32_t TrimmedWidth = 0.0f;
	};
	
	class ITextLayout : public IRefer
	{
	public:
		ITextLayout() = default;
		virtual ~ITextLayout() = default;

		virtual STextLayoutMetrics GetMetrics() const = 0;
	};
}
