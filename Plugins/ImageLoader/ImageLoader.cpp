#include "PCH.h"
#include "ImageLoader.h"
#include "FreeImage/Include/FreeImage.h"

#pragma comment(lib, "FreeImage.lib")

namespace XE
{
	TSharedPtr<FImage> FImageLoader::LoadImageFromFile(FStringView FileName)
	{
		FWStringView FileNameW = Strings::ToWide(FileName);
		FREE_IMAGE_FORMAT FiFormat = FreeImage_GetFileTypeU(FileNameW.GetData());
		if (FiFormat == FIF_UNKNOWN)
			return nullptr;
		
		FIBITMAP * FileFIBitmap = FreeImage_LoadU(FiFormat, FileNameW.GetData());
		FIBITMAP * FIBitmap = FreeImage_ConvertTo32Bits(FileFIBitmap);
		FreeImage_Unload(FileFIBitmap);
		
		if (!FIBitmap)
			return nullptr;

		uint32_t Width = FreeImage_GetWidth(FIBitmap);
		uint32_t Height = FreeImage_GetHeight(FIBitmap);
		byte_t * Bits = FreeImage_GetBits(FIBitmap);
		uint32_t Pitch = FreeImage_GetLine(FIBitmap);
		//bool FlipVertical = FreeImage_FlipVertical(FIBitmap);
		
		TSharedPtr<FImage> Image = MakeShared<FImage>(SizeU(Width, Height), EFormat::B8G8R8A8);

		for (uint32_t RowIndex = 0; RowIndex < Height; ++RowIndex)
		{
			Memcpy(Image->GetBits() + Image->GetRowPitch() * RowIndex, FreeImage_GetScanLine(FIBitmap, Height - RowIndex - 1), Pitch);
		}
		
		FreeImage_Unload(FIBitmap);
		return Image;
	}

	TSharedPtr<Engine::FTexture> FImageLoader::LoadFromFile(FStringView FileName)
	{
		FWStringView FileNameW = Strings::ToWide(FileName);
		FREE_IMAGE_FORMAT FiFormat = FreeImage_GetFileTypeU(FileNameW.GetData());
		if (FiFormat == FIF_UNKNOWN)
			return nullptr;

		FIBITMAP * FileFIBitmap = FreeImage_LoadU(FiFormat, FileNameW.GetData());
		FIBITMAP * FIBitmap = FreeImage_ConvertTo32Bits(FileFIBitmap);
		FreeImage_Unload(FileFIBitmap);

		if (!FIBitmap)
			return nullptr;

		uint32_t Width = FreeImage_GetWidth(FIBitmap);
		uint32_t Height = FreeImage_GetHeight(FIBitmap);
		byte_t * Bits = FreeImage_GetBits(FIBitmap);
		uint32_t Pitch = FreeImage_GetLine(FIBitmap);
		//bool FlipVertical = FreeImage_FlipVertical(FIBitmap);

		TSharedPtr<Engine::FTexture> Texture = MakeShared<Engine::FTexture>(Engine::FEngine::Instance());
		Texture->Format = EFormat::B8G8R8A8;
		Texture->Size = { Width, Height };
		Texture->RowPitch = Width * 4;
		Texture->Data.Resize(Width * Height * 4, Width * Height * 4, EInitializeMode::None);
		for (uint32_t RowIndex = 0; RowIndex < Height; ++RowIndex)
		{
			Memcpy(Texture->Data.Data + Texture->RowPitch * RowIndex, FreeImage_GetScanLine(FIBitmap, Height - RowIndex - 1), Pitch);
		}

		FreeImage_Unload(FIBitmap);
		return Texture;
	}
}

