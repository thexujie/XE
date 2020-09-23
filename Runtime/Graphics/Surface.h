#pragma once

#include "GraphicsInc.h"

namespace XE
{
	class FSurface : public FObject
	{
	public:
		FSurface() = default;
		~FSurface() = default;

		virtual void * GetPointer() const = 0;
		virtual EFormat GetFormat() const = 0;
		virtual SizeU GetSize() const = 0;
	};
}