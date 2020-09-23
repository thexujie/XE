#pragma once

#include "../EngineInc.h"
#include "../RenderTargets/NormalRenderTarget.h"

namespace XE::Engine
{
	class ENGINE_API FScenePass : public FRenderPass
	{
	public:
		FScenePass();

		void Update(FRenderFactory & RenderFactory, const FView & View, FRHICommandList & RHICommandList) override;
		void Excute(FRenderFactory & RenderFactory, const FView & SceneView, FRHICommandList & RHICommandList) override;

	public:
		FRHIResource * GetColorResource() const { return RenderTarget->GetColorResource(); }
		FRHIResourceView * GetColorResourceSRV() const { return RenderTarget->GetColorResourceSRV(); }
		
	private:
		TSharedPtr<FSimpleRenderTarget> RenderTarget;
		
		FViewPort ViewPort;
		RectI ScissorRect;

		FRootSignatureDesc RootSignatureArgs;
		TMap<const FPipelineState *, TVector<FRenderCommand>> RenderCommands;
	};
}
