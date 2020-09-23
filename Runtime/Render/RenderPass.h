#pragma once

#include "RenderInc.h"

namespace XE::Render
{
	using FPipelineStateCacheId = size_t;

	class RENDER_API FRenderPass : public IRenderObject
	{
	public:
		FRenderPass() = default;

		virtual void Update(FRenderFactory & RenderFactory, const FView & SceneView, RHI::FRHICommandList & RHICommandList) = 0;
		virtual void Excute(FRenderFactory & RenderFactory, const FView & SceneView, RHI::FRHICommandList & RHICommandList) = 0;
	};
}
