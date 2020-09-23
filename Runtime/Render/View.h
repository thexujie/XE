#pragma once

#include "RenderInc.h"
#include "RenderCommand.h"

namespace XE::Render
{
	class RENDER_API FView : public IRenderObject
	{
	public:
		virtual ptr_t GetWindowHandle() const = 0;
		virtual SizeU GetViewSize() const = 0;
		virtual FViewPort GetViewPort() const = 0;
		virtual FMatrix GetViewMatrix() const = 0;
		virtual FMatrix GetProjectionMatrix() const = 0;
		virtual FMatrix GetViewProjectionMatrix() const = 0;
		virtual RectI GetScissorRect() const = 0;
		virtual void Render(FRenderFactory & RenderFactory, FRHICommandQueue & RHICommandQueue, FRHICommandList & RHICommandList) = 0;

		
	public:
		TVector<FPrimitiveRenderCommand> PrimitiveRenderCommands;
		void ResetPrimitiveRenderCommand();
		FPrimitiveRenderCommand & AllocPrimitiveRenderCommand();
	};
}
