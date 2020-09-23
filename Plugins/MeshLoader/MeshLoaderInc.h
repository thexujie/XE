#pragma once

#include "Engine/Engine.h"

#if XE_WINDOWS
#ifdef XEngine_MeshLoader_Module
#define MESH_LOADER_API __declspec(dllexport)
#else
#define MESH_LOADER_API __declspec(dllimport)
#endif
#endif
