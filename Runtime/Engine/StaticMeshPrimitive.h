#pragma once

#include "EngineInc.h"
#include "ScenePrimitive.h"

namespace XE::Engine
{
	struct FMeshVertex
	{
		FVector3 Position;
		FVector2 UV;
		FVector2 UV2;
		FVector3 Normal;
		FVector3 BiNormal;
		FColor Color;

		friend FArchive & operator >>(FArchive & Serializer, FMeshVertex & MeshVertex)
		{
			Serializer >> MeshVertex.Position;
			Serializer >> MeshVertex.UV;
			Serializer >> MeshVertex.UV2;
			Serializer >> MeshVertex.Normal;
			Serializer >> MeshVertex.BiNormal;
			Serializer >> MeshVertex.Color;
			return Serializer;
		}
	};
	
	class ENGINE_API FStaticMeshPrimitive : public IScenePrimitive
	{
	public:
		FStaticMeshPrimitive(FEngine & EngineIn);
		~FStaticMeshPrimitive() = default;

		EAssetType GetAssetType() const override { return EAssetType::Mesh; }
		EError Serialize(FArchive & Archive);
		
		void OnEnterScene(FScene * Scene);

		void OnAssetLoaded_iot(IEngineResource * SceneResource, EError Error);
		void Initialize_rt(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList);
		
		void SetMaterial(uint32_t Index, IMaterial * MaterialIn);
		void RenderUpdate_rt(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList);
		void Render(FRenderFactory & RenderFactory, FView & View, FRHICommandList & RHICommandList);

		void SetLocalTransform(const FTransform & LocalTransformIn) { LocalTransform = LocalTransformIn; }
		void SetWorldTransform(const FTransform & WorldTransformIn) { WorldTransform = WorldTransformIn; }

	private:
		TPointer<const IInputLayout> InputLayout;
		TSharedPtr<FVertexBuffer> VertexBuffer;
		TSharedPtr<FRHIResource> IndexBuffer;
		TSharedPtr<FRHIResource> ConstBuffer;
		
		TSharedPtr<FResourceBindings> ResourceBindings;

		IMaterial * Material = nullptr;
		IMeshAssembler * MeshAssembler = nullptr;
		TSharedPtr<FShaderParameter> PrimitiveParemeter;

		FTransform LocalTransform = FTransform::Identity;
		FTransform WorldTransform = FTransform::Identity;

	public:
		TVector<FMeshVertex> Vertices;
		TVector<uint32_t> Indices;
		EVertexElements VertexElements = EVertexElements::None;
	};

}