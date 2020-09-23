#pragma once

#include "Graphics/Graphics.h"
#include "Engine/Engine.h"

#if XE_WINDOWS
#ifdef XEngine_ImageLoader_Module
#define IMAGE_LOADER_API __declspec(dllexport)
#else
#define IMAGE_LOADER_API __declspec(dllimport)
#endif
#endif
