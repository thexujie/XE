#pragma once

#if XE_WINDOWS
#ifdef XEngine_RHID3D12_Module
#define RHID3D12_API __declspec(dllexport)
#else
#define RHID3D12_API __declspec(dllimport)
#endif
#endif

