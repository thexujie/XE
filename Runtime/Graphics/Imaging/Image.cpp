#include "PCH.h"
#include "Image.h"

namespace XE
{
	FImage::FImage(const SizeU & SizeIn, EFormat FormatIn)
	{
		switch(FormatIn)
		{
		case EFormat::B8G8R8A8:
		case EFormat::R8G8B8A8:
		case EFormat::B8G8R8X8:
		case EFormat::R8G8B8X8:
		case EFormat::A8R8G8B8:
		case EFormat::A8B8G8R8:
		case EFormat::X8R8G8B8:
		case EFormat::X8B8G8R8:
			Format = FormatIn;
			RowCount = SizeIn.Height;
			RowPitch = SizeIn.Width * 4;
			break;
		default:
			break;
		}
		
		if (RowPitch > 0)
		{
			Size = SizeIn;
			Bits = (byte_t *)Malloc(RowPitch * RowCount);
		}
	}
	
	FImage::~FImage()
	{
		if (Bits)
		{
			Free(Bits);
			Bits = nullptr;
			Size = SizeU(0, 0);
		}
	}
}