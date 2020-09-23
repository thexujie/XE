#include "PCH.h"
#include "Scene.h"

namespace XE::Engine
{
	void FScene::AddPrimitive(IScenePrimitive * ScenePrimitive)
	{
		Primitives.Add(ScenePrimitive);
		ScenePrimitive->OnEnterScene(this);
	}

	void FScene::BeginPlay()
	{
		
	}
	
	void FScene::EndPlay()
	{
		
	}

}
