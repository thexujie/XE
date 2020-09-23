#pragma once

#include "../EngineInc.h"

namespace XE::Engine
{
	class FViewRectGeometry : public IRenderable
	{
	public:
		void Initialize(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList);
		void Render(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList);

	private:
		TPointer<IMeshAssembler> MeshAssembler;
		TPointer<IMaterial> Material;
		TPointer<FPipelineState> PipelineState;
	};
}
