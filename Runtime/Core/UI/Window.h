#pragma once

#include "UIInc.h"

namespace XE
{
	class CORE_API FWindowUtils
	{
	public:
		FWindowUtils() = delete;

		static SizeU GetWindowSize(ptr_t WindowHandle);
	};
}