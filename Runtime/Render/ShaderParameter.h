#pragma once

#include "RenderInc.h"

namespace XE::Render
{
	class RENDER_API FShaderParameter
	{
	public:
		virtual ~FShaderParameter() = default;
		virtual uint32_t GetSize() const = 0;
		virtual byte_t * GetData() = 0;
		virtual const byte_t * GetData() const = 0;
	};
}

