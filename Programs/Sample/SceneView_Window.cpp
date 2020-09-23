#include "PCH.h"
#include "SceneView_Window.h"

void FSceneView_Window::Update(ptr_t WindowHandleIn)
{
	RECT rcClient = {};
	GetClientRect(HWND(WindowHandleIn), &rcClient);

	ViewSize = FWindowUtils::GetWindowSize(WindowHandleIn);

	ViewPort.Left = 0;
	ViewPort.Top = 0;
	ViewPort.Width = float32_t(rcClient.right - rcClient.left);
	ViewPort.Height = float32_t(rcClient.bottom - rcClient.top);
	ViewPort.NearZ = 0.0f;
	ViewPort.FarZ = 1.0f;

	SccisorRect = { 0, 0, int32_t(ViewPort.Width), int32_t(ViewPort.Height) };

	WindowHandle = WindowHandleIn;
}

void FSceneView_Window::Render(FRenderFactory & RenderFactory, FRHICommandQueue & RHICommandQueue, FRHICommandList & RHICommandList)
{
	RHICommandList.Open();
	ScenePass.Update(RenderFactory, *this, RHICommandList);
	PresentPass.Update(RenderFactory, *this, RHICommandList);
	RHICommandList.Close();
	RHICommandQueue.ExcuteCommandList(&RHICommandList);

	PresentPass.Setup(ScenePass.GetColorResource());
	// excute
	ScenePass.Excute(RenderFactory, *this, RHICommandList);
	PresentPass.Excute(RenderFactory, *this, RHICommandList);

}

void FPresentPass::Update(FRenderFactory & RenderFactory, const FView & View, RHI::FRHICommandList & RHICommandList)
{
	if (!RHISwapChain || RHISwapChain->GetSize() != View.GetViewSize())
	{
		RHISwapChain.Reset();

		RHI::FSwapChainDesc SwapChainArgs = {};
		SwapChainArgs.WindowHandle = View.GetWindowHandle();
		SwapChainArgs.NumBuffers = NumBackBuffers;
		SwapChainArgs.Size = View.GetViewSize();
		RHISwapChain = RenderFactory.RHIDevice.CreateSwapChain(&RenderFactory.RHICommandQueue, SwapChainArgs);
	}
}

void FPresentPass::Excute(FRenderFactory & RenderFactory, const FView & View, RHI::FRHICommandList & RHICommandList)
{
	RHICommandList.Open();
	uint32_t FrameIndex = RHISwapChain->FrameIndex();
	RHI::FRHIResource * RHICurrentFrameResource = RHISwapChain->GetBuffer(FrameIndex);
	if (Color->GetResourceArgs().MSAA.Level > 1)
	{
		RHICommandList.TransitionBarrier(RHICurrentFrameResource, EResourceStates::ResolveDest);
		RHICommandList.TransitionBarrier(Color.Get(), EResourceStates::ResolveSource);
		RHICommandList.ResolveSubresource(RHICurrentFrameResource, 0, Color.Get(), 0, EFormat::BGRA);
		RHICommandList.TransitionBarrier(RHICurrentFrameResource, EResourceStates::Present);
	}
	else
	{
		RHICommandList.TransitionBarrier(RHICurrentFrameResource, EResourceStates::CopyDest);
		RHICommandList.TransitionBarrier(Color.Get(), EResourceStates::CopySource);
		RHICommandList.CopyResource(RHICurrentFrameResource, Color.Get());
		RHICommandList.TransitionBarrier(RHICurrentFrameResource, EResourceStates::Present);
	}
	RHICommandList.Close();
	RenderFactory.RHICommandQueue.ExcuteCommandList(&RHICommandList);
	RHISwapChain->Present(0);
}

