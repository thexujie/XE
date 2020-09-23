#pragma once

#include "GraphicsInc.h"

namespace XE
{
	class GRAPHICS_API FPixmap : public FObject
	{
	public:
		FPixmap() = default;
		~FPixmap() = default;

		virtual const SizeU & GetSize() const = 0;
		virtual TTuple<const byte_t *, uint32_t> Lock() = 0;
		virtual void UnLock() = 0;
	};
}
