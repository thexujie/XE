#pragma once

#include "Core/Core.h"


#if XE_WINDOWS
#ifdef XEngine_Graphics_Module
#define GRAPHICS_API __declspec(dllexport)
#else
#define GRAPHICS_API __declspec(dllimport)
#endif
#endif

namespace XE
{
	
}

