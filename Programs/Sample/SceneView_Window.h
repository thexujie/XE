#pragma once

#include "Inc.h"
#include "Engine/RenderPasses/ScenePass.h"

class FPresentPass : public FRenderPass
{
public:
	void Setup(FRHIResource * ColorIn) { Color = ColorIn; }
	void Update(FRenderFactory & RenderFactory, const FView & View, RHI::FRHICommandList & RHICommandList);
	void Excute(FRenderFactory & RenderFactory, const FView & View, RHI::FRHICommandList & RHICommandList);

public:
	TPointer<FRHIResource> Color;
	TSharedPtr<FRHISwapChain> RHISwapChain;
	uint32_t NumBackBuffers = 3;
};

class FSceneView_Window : public FView
{
public:
	FSceneView_Window() = default;

	ptr_t GetWindowHandle() const override { return WindowHandle; }
	SizeU GetViewSize() const { return ViewSize; }
	FViewPort GetViewPort() const override { return ViewPort; }
	RectI GetScissorRect() const override { return SccisorRect; }
	FMatrix GetViewMatrix() const { return ViewMatrix; }
	FMatrix GetProjectionMatrix() const { return ProjectionMatrix; }
	FMatrix GetViewProjectionMatrix() const { return ViewProjectionMatrix; }

	ptr_t WindowHandle = nullptr;
	SizeU ViewSize;
	FViewPort ViewPort;
	RectI SccisorRect;
	FMatrix ViewMatrix;
	FMatrix ProjectionMatrix;
	FMatrix ViewProjectionMatrix;

	void Update(ptr_t WindowHandle);

private:
	FScenePass ScenePass;
	FPresentPass PresentPass;

	void Render(FRenderFactory & RenderFactory, FRHICommandQueue & RHICommandQueue, FRHICommandList & RHICommandList);
};
