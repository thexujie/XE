#include "PCH.h"
#include "StaticMeshPrimitive.h"

#include "Engine.h"
#include "VertexBuffer.h"

namespace XE::Engine
{
	struct SPrimitiveParemeters
	{
		float4x4 LocalTransform;
		float4x4 WorldTransform;
		float4x4 ViewTransform;
		float4x4 ProjectionTransform;
		float4x4 ViewProjectionTransform;
		float4x4 WorldViewProjectionTransform;
	};

	FStaticMeshPrimitive::FStaticMeshPrimitive(FEngine & EngineIn) : IScenePrimitive(EngineIn)
	{
		MeshAssembler = &BaseShader_LocalMeshAssembler;
		ResourceBindings = MakeShared<FResourceBindings>();
	}

	EError FStaticMeshPrimitive::Serialize(FArchive & Archive)
	{
		Archive >> Vertices;
		Archive >> Indices;
		Archive >> VertexElements;
		return EError::OK;
	}
	
	void FStaticMeshPrimitive::OnEnterScene(FScene * Scene)
	{
		
	}

	void FStaticMeshPrimitive::OnAssetLoaded_iot(IEngineResource * SceneResource, EError Error)
	{
		Assert(SceneResource == this);
		Engine.EnqueueCommand([this](FRenderFactory & RenderFactory, FRHICommandList & RHICommandList)
			{
				Initialize_rt(RenderFactory, RHICommandList);
			});
	}

	void FStaticMeshPrimitive::Initialize_rt(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList)
	{
		VertexBuffer = MakeShared<FVertexBuffer>(VertexElements);
		VertexBuffer->InitializeRHI(RenderFactory, RHICommandList, Vertices.Data, sizeof(FMeshVertex) * Vertices.GetSize());
		ResourceBindings->Update_rt(RenderFactory, RHICommandList);

		{
			FResourceDesc ResourceDesc;
			ResourceDesc.Size = SizeU(sizeof(uint32_t) * (uint32_t)Indices.GetSize(), 1);
			ResourceDesc.Dimension = EResourceDimension::Buffer;
			ResourceDesc.Usages = EResourceUsage::IndexBuffer;
			IndexBuffer = RenderFactory.RHIDevice.CreateResource(ResourceDesc);

			FResourceAddress ResourceAddress = RenderFactory.FetchBufferCPU(sizeof(uint32_t) * (uint32_t)Indices.GetSize());
			Memcpy(ResourceAddress.CPUPointer, Indices.GetData(), sizeof(uint32_t) * Indices.GetSize());
			RHICommandList.TransitionBarrier(IndexBuffer.Get(), EResourceStates::CopyDest);
			RHICommandList.CopyResourceRegion(IndexBuffer.Get(), 0, ResourceAddress.RHIResource, ResourceAddress.Offset, ResourceAddress.NumBytes);
			RHICommandList.TransitionBarrier(IndexBuffer.Get(), EResourceStates::IndexBuffer);
		}

		ConstBuffer = RenderFactory.RHIDevice.CreateResource(FResourceDesc::Buffer(EHeapType::Default, AlignUp(sizeof(SPrimitiveParemeters), 0xFF)));
		InputLayout = RenderFactory.GetInputLayout(EVertexElements::Position | EVertexElements::UV);
		OnInitializedRHI.Excute(this, EError::OK);
	}

	void FStaticMeshPrimitive::RenderUpdate_rt(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList)
	{
	}

	void FStaticMeshPrimitive::SetMaterial(uint32_t Index, IMaterial * MaterialIn)
	{
		Material = MaterialIn;
	}
	
	void FStaticMeshPrimitive::Render(FRenderFactory & RenderFactory, FView & View, FRHICommandList & RHICommandList)
	{
		if (!InputLayout)
			return;

		Material->BindResources(*ResourceBindings);
		ResourceBindings->SetConstBuffer(EShaderType::Vertex, 0, ConstBuffer.Get());
		ResourceBindings->Update_rt(RenderFactory, RHICommandList);
		
		FPrimitiveRenderCommand & PrimitiveRenderCommand = View.AllocPrimitiveRenderCommand();

		PrimitiveRenderCommand.InputLayout = InputLayout.Get();
		PrimitiveRenderCommand.MeshAssembler = MeshAssembler;
		PrimitiveRenderCommand.Material = Material;

		PrimitiveRenderCommand.ResourceBindings = ResourceBindings.Get();
		{
			FResourceAddress ResourceAddress = RenderFactory.FetchBufferCPU(sizeof(SPrimitiveParemeters), 0xff);
			SPrimitiveParemeters & PrimitiveParemeters = *(SPrimitiveParemeters *)ResourceAddress.CPUPointer;
			PrimitiveParemeters.LocalTransform = LocalTransform.ToMatrix();
			PrimitiveParemeters.WorldTransform = PrimitiveParemeters.LocalTransform * WorldTransform.ToMatrix();
			PrimitiveParemeters.WorldViewProjectionTransform = PrimitiveParemeters.WorldTransform * View.GetViewMatrix() * View.GetProjectionMatrix();

			RHICommandList.TransitionBarrier(ConstBuffer.Get(), EResourceStates::CopyDest);
			RHICommandList.CopyResourceRegion(ConstBuffer.Get(), 0, ResourceAddress.RHIResource, ResourceAddress.Offset, sizeof(SPrimitiveParemeters));
			RHICommandList.TransitionBarrier(ConstBuffer.Get(), EResourceStates::ConstBuffer);
		}

		PrimitiveRenderCommand.Transform = WorldTransform;
		PrimitiveRenderCommand.Topology = ETopology::TriangleList;
		PrimitiveRenderCommand.VertexBuffer = VertexBuffer->GetRHIResource();
		PrimitiveRenderCommand.IndexBuffer = IndexBuffer.Get();
		PrimitiveRenderCommand.VertexStride = sizeof(FMeshVertex);
		PrimitiveRenderCommand.IndexStride = sizeof(uint32_t);
		PrimitiveRenderCommand.NumVertices = (uint32_t)Vertices.GetSize();
		PrimitiveRenderCommand.NumIndices = (uint32_t)Indices.GetSize();
		PrimitiveRenderCommand.NumInstances = 1;
	}
}
