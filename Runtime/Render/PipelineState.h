#pragma once

#include "RenderInc.h"

namespace XE::Render
{
	class RENDER_API FPipelineState
	{
	public:
		FPipelineState(const IInputLayout & VertexLayoutIn, const IOutputLayout & OutputLayoutIn, const IMeshAssembler & MeshAssemblerIn, const IMaterial & MaterialIn)
		: InputLayout(VertexLayoutIn), OutputLayout(OutputLayoutIn), MeshAssembler(MeshAssemblerIn), Material(MaterialIn) {}

		void InitializeRHI(RHI::FRHIDevice & RHIDevice, RHI::FRHIRootSignature * RHIRootSignature);
		
	public:
		RHI::FRHIPipelineState * GetRHIPipelineState() const { return RHIPipelineState.Get(); }
		
	private:
		const IInputLayout & InputLayout;
		const IOutputLayout & OutputLayout;
		const IMeshAssembler & MeshAssembler;
		const IMaterial & Material;
		
		TSharedPtr<RHI::FRHIPipelineState> RHIPipelineState;
	};
}
