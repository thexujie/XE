#pragma once

#include "Core/Core.h"
#include "Core/Platform/Win32/Win32.h"
#include "Core.Windows/CoreWindows.h"

using namespace XE;
using namespace XE::Win32;
using namespace XE::Windows;

#include <shlobj.h>
#include <shlwapi.h>

#if XE_WINDOWS
#ifdef XEngine_NavigateToOriginalPath_Module
#define NavigateToOriginalPath_API __declspec(dllexport)
#else
#define NavigateToOriginalPath_API __declspec(dllimport)
#endif
#endif

extern XE::uintx_t DllRefCount;
