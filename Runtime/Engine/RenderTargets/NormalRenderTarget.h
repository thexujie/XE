#pragma once

#include "../EngineInc.h"

namespace XE::Engine
{
	class ENGINE_API FSimpleRenderTarget : public IRenderTarget
	{
	public:
		FSimpleRenderTarget();


		void InitializeRHI(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList);
		
		void Update(FRenderFactory & RenderFactory, const FView & SceneView, FRHICommandList & RHICommandList);

		void Bind(FRHICommandList & RHICommandList);
		void UnBind(FRHICommandList & RHICommandList);

		const IOutputLayout * GetOutputLayout() const { return OutputLayout.Get(); }
		
	public:
		FRHIResource * GetColorResource() const { return ColorResource.Get(); }
		FRHIResourceView * GetColorResourceSRV() const { return ColorResourceView.Get(); }
		
	private:
		SizeU Size;
		FMSAA MSAA;
		FColor ClearColor = Colors::Bisque;

		TPointer<const IOutputLayout> OutputLayout;
		
		TSharedPtr<FRHIResource> ColorResource;
		TSharedPtr<FRHIResource> DepthStencil;

		TSharedPtr<FRHIResourceView> ColorResourceView;
		TSharedPtr<FRHIResourceView> DepthStencilView;
	};
}
