#include "PCH.h"
#include "Texture.h"
#include "Engine.h"

namespace XE::Engine
{
	FTexture::FTexture(FEngine & EngineIn, FStringView AssetFileName) :IEngineResource(EngineIn)
	{
		LoadAsset_any(AssetFileName);
	}
	
	void FTexture::OnAssetLoaded_iot(IEngineResource * SceneResource, EError Error)
	{
		Assert(SceneResource == this);
		Engine.EnqueueCommand([this](FRenderFactory & RenderFactory, FRHICommandList & RHICommandList)
			{
				Initialize_rt(RenderFactory, RHICommandList);
			});
	}
	
	EError FTexture::Serialize(FArchive & Archive)
	{
		Archive >> Format;
		Archive >> Size;
		Archive >> RowPitch;
		Archive >> Data;
		return EError::OK;
	}
	
	void FTexture::Initialize_rt(FRenderFactory & RenderFactory, FRHICommandList & RHICommandList)
	{
		FResourceDesc ResourceArgs;
		ResourceArgs.Size = Size;
		ResourceArgs.Dimension = EResourceDimension::Texture2D;
		ResourceArgs.Usages = EResourceUsage::ShaderResource;
		ResourceArgs.Format = Format;
		Texture = RenderFactory.RHIDevice.CreateResource(ResourceArgs);
		{
			FCopyableFootprint CopyableFootprint = RenderFactory.RHIDevice.GetCopyableFootprint(Texture.Get());
			assert(CopyableFootprint.NumRows == Size.Height);
			assert(CopyableFootprint.RowPitch >= RowPitch);
			FResourceAddress ResourceAddress = RenderFactory.FetchBufferCPU(CopyableFootprint.NumRows * CopyableFootprint.RowPitch, CopyableFootprint.AlignMask);
			for (uint32_t RowIndex = 0; RowIndex < CopyableFootprint.NumRows; ++RowIndex)
			{
				Memcpy(ResourceAddress.CPUPointer + RowIndex * CopyableFootprint.RowPitch, Data.GetData() + RowPitch * RowIndex, RowPitch);
				//Memset(ResourceAddress.CPUPointer + RowIndex * CopyableFootprint.RowPitch, 0xff, CopyableFootprint.RowPitch);
			}
			RHICommandList.TransitionBarrier(Texture.Get(), EResourceStates::CopyDest);
			{
				CopyableFootprint.Offset = uint32_t(ResourceAddress.Offset);
				RHICommandList.CopyTexture(Texture.Get(), ResourceAddress.RHIResource, CopyableFootprint);
			}
			RHICommandList.TransitionBarrier(Texture.Get(), EResourceStates::PixelShaderResource);
		}
		OnInitializedRHI.Excute(this, EError::OK);
	}
}

