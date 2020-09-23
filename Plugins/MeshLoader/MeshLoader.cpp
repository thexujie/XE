#include "PCH.h"
#include "MeshLoader.h"
#include "FbxLoader.h"


namespace XE
{
	TSharedPtr<Engine::FStaticMeshPrimitive> FMeshLoader::LoadStaticMeshFromFile(FStringView FileName)
	{
		FFbxLoadConfig LoadConfig = {};
		FFbxLoader Loader;
		return Loader.LoadMeshFromFile(FileName, LoadConfig);
	}
}
