#pragma once

#include "EngineInc.h"

namespace XE::Engine
{
	class ENGINE_API IEngineObject
	{
	public:
		virtual ~IEngineObject() = default;

		void SetName(FStringView NameIn) { Name = NameIn; }
		FStringView GetName() const { return Name; }
		
	protected:
		FString Name;
	};
	
	class ENGINE_API IEngineResource : public IEngineObject
	{
	public:
		IEngineResource(FEngine & EngineIn) : Engine(EngineIn) {}

		virtual EAssetType GetAssetType() const = 0;
		virtual EError Serialize(FArchive & Archive) = 0;
		
		virtual void OnAssetLoaded_iot(IEngineResource * SceneResource, EError Error) = 0;
		virtual void Initialize_rt(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList) = 0;

		void LoadAsset_any(FStringView AssetPath);

	protected:
		FEngine & Engine;
		TEvent<IEngineResource *, EError> OnInitializedRHI;
	};
}
