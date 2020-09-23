#pragma once

#include "EngineInc.h"

namespace XE::Engine
{
	class FUserInterface : public FObject
	{
	public:
		FUserInterface() = default;

		void Update(FRenderFactory & RenderFactory, const FView & View);
		void FlushToTexture(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList);

	public:
		FRHIResource * GetBackBufferRHIResource() const { return Texture.Get(); }
		
	private:
		TSharedPtr<FPixmap> Bitmap;
		TSharedPtr<FGraphics> Graphics;
		TSharedPtr<FRHIResource> Texture;
	};

}