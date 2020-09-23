#pragma once

#include "Core/Core.h"
#include "Core/Platform/Win32/Win32.h"

#if XE_WINDOWS
#ifdef XEngine_Core_Windows_Module
#define CORE_WINDOWS_API __declspec(dllexport)
#else
#define CORE_WINDOWS_API __declspec(dllimport)
#endif
#endif
