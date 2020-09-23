#include "PCH.h"
#include "EngineResource.h"
#include "Engine.h"

namespace XE::Engine
{
	void IEngineResource::LoadAsset_any(FStringView AssetPath)
	{
		Engine.GetAssetFactory().LoadAsset_any(AssetPath, this, TFunction<void(IEngineResource *, EError)>(&IEngineResource::OnAssetLoaded_iot, this));
	}
}
