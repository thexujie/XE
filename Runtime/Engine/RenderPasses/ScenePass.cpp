#include "PCH.h"
#include "ScenePass.h"
#include "Engine/ScenePrimitive.h"
#include "Engine/VertexBuffer.h"

namespace XE::Engine
{
	FScenePass::FScenePass()
	{
		RenderTarget = MakeShared<FSimpleRenderTarget>();
	}
	
	void FScenePass::Update(FRenderFactory & RenderFactory, const FView & View, FRHICommandList & RHICommandList)
	{
		RenderTarget->Update(RenderFactory, View, RHICommandList);
		RenderCommands.Destroy();
		
		for (FPrimitiveRenderCommand & PrimitiveRenderCommand : View.PrimitiveRenderCommands)
		{
			const FPipelineState * PipelineState = RenderFactory.GetPipelineState(PrimitiveRenderCommand.InputLayout, RenderTarget->GetOutputLayout(), PrimitiveRenderCommand.MeshAssembler, PrimitiveRenderCommand.Material);
			FRenderCommand & RenderCommand = RenderCommands[PipelineState].Emplace();

			RenderCommand.PipelineState = PipelineState;
			RenderCommand.ResourceBindings = PrimitiveRenderCommand.ResourceBindings;
			
			RenderCommand.Topology = PrimitiveRenderCommand.Topology;
			RenderCommand.VertexBuffer = PrimitiveRenderCommand.VertexBuffer;
			RenderCommand.IndexBuffer = PrimitiveRenderCommand.IndexBuffer;
			RenderCommand.NumVertices = PrimitiveRenderCommand.NumVertices;
			RenderCommand.NumIndices = PrimitiveRenderCommand.NumIndices;
			RenderCommand.VertexStride = PrimitiveRenderCommand.VertexStride;
			RenderCommand.IndexStride = PrimitiveRenderCommand.IndexStride;
			RenderCommand.VertexIndexBase = PrimitiveRenderCommand.VertexIndexBase;
			RenderCommand.IndexIndexBase = PrimitiveRenderCommand.IndexIndexBase;
			RenderCommand.NumInstances = PrimitiveRenderCommand.NumInstances;
			RenderCommand.InstanceIndexBase = PrimitiveRenderCommand.InstanceIndexBase;
		}
	}
	
	void FScenePass::Excute(FRenderFactory & RenderFactory, const FView & SceneView, FRHICommandList & RHICommandList)
	{
		RHICommandList.Open();
		RHICommandList.SetViewPort(SceneView.GetViewPort());
		RHICommandList.SetScissorRect(SceneView.GetScissorRect());

		RenderTarget->Bind(RHICommandList);
		
		//RHICommandList.SetRootDescriptorTable();
		//RHICommandList.SetRenderTarget({ SceneTextureView.Get() }, DepthStencilView.Get());

		RHICommandList.SetRootSignature(RenderFactory.GetLocalRootSignature());
		for (auto RenderCommandIter : RenderCommands)
		{
			FRHIPipelineState * RHIPipelineState = RenderCommandIter.Key->GetRHIPipelineState();
			RHICommandList.SetPipelineState(RHIPipelineState);
			for (auto & RenderCommand : RenderCommandIter.Value)
			{
				RHICommandList.IASetTopology(RenderCommand.Topology);
				RenderCommand.ResourceBindings->Bind(RHICommandList);
				
				RHICommandList.IASetVertexBuffer(RenderCommand.VertexBuffer, RenderCommand.VertexStride, RenderCommand.NumVertices);
				if (RenderCommand.IndexBuffer)
				{
					RHICommandList.IASetIndexBuffer(RenderCommand.IndexBuffer, RenderCommand.IndexStride, RenderCommand.NumIndices);
					RHICommandList.DrawIndexedInstanced(RenderCommand.NumIndices, RenderCommand.NumInstances, RenderCommand.IndexIndexBase, RenderCommand.VertexIndexBase, RenderCommand.InstanceIndexBase);
				}
				else
				{
					RHICommandList.DrawInstanced(RenderCommand.NumVertices, RenderCommand.NumInstances, RenderCommand.VertexIndexBase, RenderCommand.InstanceIndexBase);
				}
			}
		}
		RHICommandList.Close();
		RenderFactory.RHICommandQueue.ExcuteCommandList(&RHICommandList);
	}
}
