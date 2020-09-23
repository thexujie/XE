#include "PCH.h"
#include "MeshLoader/MeshLoader.h"
#include "ImageLoader/ImageLoader.h"

using namespace XE;
using namespace XE::Engine;

void ImportMesh(FAssetFactory & AssetFactory, FStringView Source, FStringView Dest)
{
	XE::TSharedPtr<Engine::FStaticMeshPrimitive> StaticMesh = FMeshLoader::LoadStaticMeshFromFile(Source);
	AssetFactory.SaveResource(Dest, *StaticMesh);
}

void ImportTexture(FAssetFactory & AssetFactory, FStringView Source, FStringView Dest)
{
	XE::TSharedPtr<FTexture> TextureAsset = FImageLoader::LoadFromFile(Source);
	AssetFactory.SaveResource(Dest, *TextureAsset);
}

int _tmain(int argc, const char *args[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(399);

	FAssetFactory AssetFactory;
	AssetFactory.SaveResource(Str("../Content/Cube.xmsh"), *FMeshLoader::LoadStaticMeshFromFile(Str("../Data/Cube.fbx")));
	AssetFactory.SaveResource(Str("../Content/Wolf.xmsh"), *FMeshLoader::LoadStaticMeshFromFile(Str("../Data/Wolf.fbx")));

	AssetFactory.SaveResource(Str("../Content/Wolf_Body.xtex"), *FImageLoader::LoadFromFile(Str("../Data/Wolf_Body.jpg")));
	AssetFactory.SaveResource(Str("../Content/Cats.xtex"), *FImageLoader::LoadFromFile(Str("../Data/Cats.jpg")));
	
	auto TextureAsset = AssetFactory.LoadAsset(Str("../Content/Cats.xtex"));
	
	return 0;
}
