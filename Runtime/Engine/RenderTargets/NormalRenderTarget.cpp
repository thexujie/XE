#include "PCH.h"
#include "NormalRenderTarget.h"

#include "Engine.h"

namespace XE::Engine
{
	FSimpleRenderTarget::FSimpleRenderTarget()
	{
		OutputLayout = FEngine::Instance().GetRenderFactory().GetOutputLayout({ EFormat::BGRA }, EFormat::D24S8, MSAA);
	}
	
	void FSimpleRenderTarget::InitializeRHI(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList)
	{
	}
	
	void FSimpleRenderTarget::Update(FRenderFactory & RenderFactory, const FView & SceneView, FRHICommandList & RHICommandList)
	{
		if (ColorResource && Size == SceneView.GetViewSize())
			return;

		Size = SceneView.GetViewSize();
		ColorResource.Reset();
		DepthStencil.Reset();
		{
			FResourceDesc ResourceArgs = {};
			ResourceArgs.Dimension = EResourceDimension::Texture2D;
			ResourceArgs.Size = SceneView.GetViewSize();
			ResourceArgs.Format = EFormat::BGRA;
			ResourceArgs.Usages = EResourceUsage::ShaderResource | EResourceUsage::RenderTarget;
			ResourceArgs.MSAA = MSAA;
			ResourceArgs.States = EResourceStates::RenderTarget;
			ResourceArgs.ClearColor = ClearColor;
			ColorResource = RenderFactory.RHIDevice.CreateResource(ResourceArgs);
			ColorResourceView = RenderFactory.RHIDevice.CreateResourceView(ColorResource.Get(), FResourceViewArgs::RenderTarget(EFormat::BGRA));
		}

		{
			FResourceDesc ResourceArgs = {};
			ResourceArgs.Dimension = EResourceDimension::Texture2D;
			ResourceArgs.Size = SceneView.GetViewSize();
			ResourceArgs.Format = EFormat::D24S8;
			ResourceArgs.Usages = EResourceUsage::DepthStencial;
			ResourceArgs.MSAA = MSAA;
			ResourceArgs.States = EResourceStates::DepthWrite;
			ResourceArgs.ClearDepth = 1.0f;
			ResourceArgs.CleanStencial = 0;
			DepthStencil = RenderFactory.RHIDevice.CreateResource(ResourceArgs);
			DepthStencilView = RenderFactory.RHIDevice.CreateResourceView(DepthStencil.Get(), FResourceViewArgs::DepthStencil(EFormat::D24S8));
		}
	}

	void FSimpleRenderTarget::Bind(FRHICommandList & RHICommandList)
	{
		RHICommandList.TransitionBarrier(ColorResource.Get(), EResourceStates::RenderTarget);
		RHICommandList.ClearRenderTarget(ColorResourceView.Get(), ClearColor);
		RHICommandList.ClearDepthStencil(DepthStencilView.Get(), 1.0f, 0);
		RHICommandList.SetRenderTarget({ ColorResourceView.Get() }, DepthStencilView.Get());
	}

	void FSimpleRenderTarget::UnBind(FRHICommandList & RHICommandList)
	{
		
	}
}
