#pragma once

#include "MeshLoaderInc.h"

namespace XE
{
	class MESH_LOADER_API FMeshLoader
	{
	public:
		FMeshLoader() = default;
		
		static TSharedPtr<Engine::FStaticMeshPrimitive> LoadStaticMeshFromFile(FStringView FileName);
	};
}