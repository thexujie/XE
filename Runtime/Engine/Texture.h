#pragma once

#include "EngineInc.h"
#include "EngineResource.h"

namespace XE::Engine
{
	class ENGINE_API FTexture : public IEngineResource
	{
	public:
		FTexture(FEngine & EngineIn) : IEngineResource(EngineIn) {}
		FTexture(FEngine & EngineIn, FStringView AssetFileName);
		~FTexture() = default;

		EAssetType GetAssetType() const override { return EAssetType::Texture; }
		EError Serialize(FArchive & Archive) override;
		
		void OnAssetLoaded_iot(IEngineResource * SceneResource, EError Error);
		void Initialize_rt(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList);

		FRHIResource * GetRHIResource() const { return Texture.Get(); }
		
	public:
		EFormat Format = EFormat::None;
		SizeU Size = {};
		uint32_t RowPitch = 0;
		TVector<byte_t> Data;
		
	private:
		TSharedPtr<FRHIResource> Texture;
	};
}
