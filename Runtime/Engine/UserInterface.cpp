#include "PCH.h"
#include "UserInterface.h"

namespace XE::Engine
{
	void FUserInterface::Update(FRenderFactory & RenderFactory, const FView & View)
	{
		if (!Bitmap || View.GetViewSize() != Bitmap->GetSize())
		{
			FGraphicsService * GraphicsService = GApplication->GetService<FGraphicsService>();
			Bitmap = GraphicsService->CreateBitmap(View.GetViewSize(), EFormat::BGRA);
			Graphics = GraphicsService->CreateGraphics(*Bitmap);
			{
				FResourceDesc ResourceArgs = {};
				ResourceArgs.Dimension = EResourceDimension::Texture2D;
				ResourceArgs.Size = View.GetViewSize();
				ResourceArgs.Format = EFormat::BGRA;
				ResourceArgs.Usages = EResourceUsage::RenderTarget;
				ResourceArgs.MSAA = { 1, 0 };
				ResourceArgs.States = EResourceStates::RenderTarget;
				ResourceArgs.ClearColor = Colors::None;
				Texture = RenderFactory.RHIDevice.CreateResource(ResourceArgs);
			}
		}
	}

	void FUserInterface::FlushToTexture(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList)
	{
		Graphics->Clear(Colors::None);
		Graphics->DrawRectangle({ 100, 100, 640, 360 }, FSolidBrush{ Colors::Brown });
		Graphics->Flush();
		
		auto [BitmapBits, BitmapPitch] = Bitmap->Lock();
		SizeU ViewSize = Bitmap->GetSize();

		FCopyableFootprint Footprint = RenderFactory.RHIDevice.GetCopyableFootprint(Texture.Get());
		FResourceAddress ResourceAddress = RenderFactory.FetchBufferCPU(Footprint.RowPitch * Footprint.NumRows);
		for (size_t RowIndex = 0; RowIndex < ViewSize.Height; ++RowIndex)
			Memcpy(ResourceAddress.CPUPointer + RowIndex * Footprint.RowPitch, BitmapBits + RowIndex * BitmapPitch, BitmapPitch);
		Bitmap->UnLock();

		RHICommandList.TransitionBarrier(Texture.Get(), EResourceStates::CopyDest);
		//RHICommandList.CopyTextureRegion(Texture.Get(), { 0, 0, 0 }, ResourceAddress.RHIResource, { 0, 0, 0 }, { ViewSize.Width, ViewSize.Height, 1 });
		RHICommandList.CopyTexture(Texture.Get(), ResourceAddress.RHIResource, Footprint);
		RHICommandList.TransitionBarrier(Texture.Get(), EResourceStates::RenderTarget);
	}
}
