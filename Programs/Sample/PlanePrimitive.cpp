#include "PCH.h"
#include "PlanePrimitive.h"

#include "ImageLoader/ImageLoader.h"

struct SPrimitiveParemeters
{
	float4x4 LocalTransform;
	float4x4 WorldTransform;
	float4x4 ViewTransform;
	float4x4 ProjectionTransform;
	float4x4 ViewProjectionTransform;
	
	float4x4 WorldViewProjectionTransform;
};

FPlanePrimitive::FPlanePrimitive(FEngine & EngineIn) : IScenePrimitive(EngineIn)
{
	MeshAssembler = &BaseShader_LocalMeshAssembler;
	ResourceBindings = MakeShared<FResourceBindings>();
}

void FPlanePrimitive::Initialize_rt(FRenderFactory & RenderFactory, RHI::FRHICommandList & RHICommandList)
{
	TSharedPtr<FVertex> Vertices = TSharedPtr<FVertex>(new FVertex[6]);
	Vertices[0] = { FVector3(-0.5f, 0.0f, +0.5f), FVector2(0.f, 0.f) };
	Vertices[1] = { FVector3(+0.5f, 0.0f, +0.5f), FVector2(1.f, 0.f) };
	Vertices[2] = { FVector3(-0.5f, 0.0f, -0.5f), FVector2(0.f, 1.f) };

	Vertices[3] = { FVector3(+0.5f, 0.0f, +0.5f), FVector2(1.f, 0.f) };
	Vertices[4] = { FVector3(+0.5f, 0.0f, -0.5f), FVector2(1.f, 1.f) };
	Vertices[5] = { FVector3(-0.5f, 0.0f, -0.5f), FVector2(0.f, 1.f) };

	RHI::FResourceDesc ResourceArgs;
	ResourceArgs.Size = SizeU(sizeof(FVertex) * 6, 1);
	ResourceArgs.Dimension = EResourceDimension::Buffer;
	ResourceArgs.Usages = EResourceUsage::VertexBuffer;
	VertexBuffer = RenderFactory.RHIDevice.CreateResource(ResourceArgs);

	FResourceAddress ResourceAddress = RenderFactory.FetchBufferCPU(sizeof(FVertex) * 6);
	Memcpy(ResourceAddress.CPUPointer, Vertices.Get(), sizeof(FVertex) * 6);
	RHICommandList.TransitionBarrier(VertexBuffer.Get(), EResourceStates::CopyDest);
	RHICommandList.CopyResourceRegion(VertexBuffer.Get(), 0, ResourceAddress.RHIResource, ResourceAddress.Offset, ResourceAddress.NumBytes);
	RHICommandList.TransitionBarrier(VertexBuffer.Get(), EResourceStates::VertexShaderResource);

	InputLayout = RenderFactory.GetInputLayout(EVertexElements::Position | EVertexElements::UV);
	
	Material.SetTexture(MakeShared<FTexture>(Engine, Str("../Content/Cats.xtex")));
	Material.Initialize_rt(RenderFactory, RHICommandList);

	ConstBuffer = RenderFactory.RHIDevice.CreateResource(RHI::FResourceDesc::Buffer(EHeapType::Default, AlignUp(sizeof(SPrimitiveParemeters), 0xFF)));
}

void FPlanePrimitive::Render(FRenderFactory & RenderFactory, FView & View, RHI::FRHICommandList & RHICommandList)
{
	if (!InputLayout)
		return;

	Material.BindResources(*ResourceBindings);
	ResourceBindings->SetConstBuffer(EShaderType::Vertex, 0, ConstBuffer.Get());
	ResourceBindings->Update_rt(RenderFactory, RHICommandList);
	
	FPrimitiveRenderCommand & PrimitiveRenderCommand = View.AllocPrimitiveRenderCommand();

	PrimitiveRenderCommand.InputLayout = InputLayout.Get();
	PrimitiveRenderCommand.MeshAssembler = MeshAssembler.Get();
	PrimitiveRenderCommand.Material = &Material;

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
	PrimitiveRenderCommand.VertexBuffer = VertexBuffer.Get();
	PrimitiveRenderCommand.IndexBuffer = nullptr;
	PrimitiveRenderCommand.VertexStride = sizeof(FVertex);
	PrimitiveRenderCommand.NumVertices = 6;
	PrimitiveRenderCommand.NumInstances = 1;
}

