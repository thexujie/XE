#include "PCH.h"
#include "ViewRectGeometry.h"

namespace XE::Engine
{
	void FViewRectGeometry::Initialize(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList)
	{
	}

	void FViewRectGeometry::Render(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList)
	{
		RHICommandList.SetPipelineState(PipelineState->GetRHIPipelineState());
		RHICommandList.IASetVertexBuffer(nullptr, 0, 0);
		RHICommandList.DrawInstanced(6, 1, 0, 0);
	}
}
	