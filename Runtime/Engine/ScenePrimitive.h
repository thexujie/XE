#pragma once

#include "EngineInc.h"
#include "EngineResource.h"

namespace XE::Engine
{
	struct FScenePrimitiveCBuffer
	{
		FMatrix WorldTransform;
	};
	
	class ENGINE_API IScenePrimitive : public IEngineResource
	{
	public:
		IScenePrimitive(FEngine & EngineIn) : IEngineResource(EngineIn) {}
		virtual ~IScenePrimitive() = default;

		virtual EError Serialize(FArchive & Archive) override { return EError::OK; }
		
		virtual void OnEnterScene(FScene * SceneIn) { Scene = SceneIn; }
		virtual void OnLeaveScene() { Scene = nullptr; }
		
		virtual void Render(FRenderFactory & RenderFactory, FView & View, FRHICommandList & RHICommandList) = 0;

	protected:
		FScene * Scene = nullptr;
	};
}
