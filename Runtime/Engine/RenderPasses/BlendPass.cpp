#include "PCH.h"
#include "BlendPass.h"

namespace XE::Engine
{
	FLocalMeshAssembler CopyMeshAssembler(Str("../Content/Shaders/Copy.hlsl"));
	
	void FBlendPass::Initialize(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList)
	{
		ViewRectGeometry = MakeShared<FViewRectGeometry>();
	}

	void FBlendPass::Excute(FRenderFactory & RenderFactory, const FView & SceneView, FRHICommandList & RHICommandList)
	{
		RHICommandList.SetRenderTarget({ Destination.Get() }, 0);
		RHICommandList.SetRootShaderResource(0, Source.Get());
		ViewRectGeometry->Render(RenderFactory, RHICommandList);
		RHICommandList.IASetVertexBuffer(nullptr, 0, 6);
		RHICommandList.DrawInstanced(6, 1, 0, 1);
	}
}
