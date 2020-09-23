#pragma once

#include "HairShaders.h"
#include "Inc.h"
#include "Engine/Materials/TextureMaterial.h"

class FPlanePrimitive : public IScenePrimitive
{
public:
	FPlanePrimitive(FEngine & EngineIn);
	~FPlanePrimitive() = default;

	EAssetType GetAssetType() const { return EAssetType::Mesh; }
	EError Serialize(FArchive & Archive) { return EError::OK; }
	
	void OnAssetLoaded_iot(IEngineResource * SceneResource, EError Error) override {}
	void Initialize_rt(FRenderFactory & RenderFactory, RHI::FRHICommandList & RHICommandList);
	
	void Render(FRenderFactory & RenderFactory, FView & View, RHI::FRHICommandList & RHICommandList) ;

	void SetLocalTransform(const FTransform & LocalTransformIn) { LocalTransform = LocalTransformIn; }
	void SetWorldTransform(const FTransform & WorldTransformIn) { WorldTransform = WorldTransformIn; }
	
private:
	TPointer<const IInputLayout> InputLayout;
	TPointer<const IMeshAssembler> MeshAssembler;

	TSharedPtr<FResourceBindings> ResourceBindings;
	TSharedPtr<RHI::FRHIResource> VertexBuffer;
	TSharedPtr<RHI::FRHIResource> ConstBuffer;
	
	FTextureMaterial Material;
	TSharedPtr<FShaderParameter> PrimitiveParemeter;

	FTransform LocalTransform = FTransform::Identity;
	FTransform WorldTransform = FTransform::Identity;
};
