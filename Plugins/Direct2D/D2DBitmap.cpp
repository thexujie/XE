#include "PCH.h"
#include "D2DBitmap.h"
#include "D2DDevice.h"

namespace XE::Direct2D
{
	static REFWICPixelFormatGUID REFWICPixelFormatGUIDFromFormat(EFormat Format)
	{
		switch (Format)
		{
		case EFormat::BGRA:
			return GUID_WICPixelFormat32bppPBGRA;
		case EFormat::RGBA:
			return GUID_WICPixelFormat32bppPRGBA;
		case EFormat::BGRX:
			return GUID_WICPixelFormat32bppBGR;
		case EFormat::BGR:
			return GUID_WICPixelFormat24bppBGR;
		default:
			return GUID_WICPixelFormatDontCare;
		}
	}
	
	FD2DBitmap::FD2DBitmap(FD2DDevice & D2DDeviceIn, const SizeU & SizeIn, EFormat FormatIn) : D2DDevice(D2DDeviceIn), Size(SizeIn), Format(FormatIn)
	{
		WICBitmapCreateCacheOption BitmapCreateCacheOption =WICBitmapCacheOnDemand;
		HRESULT Result = D2DDevice.GetWICImagingFactory()->CreateBitmap(SizeIn.Width, SizeIn.Height, REFWICPixelFormatGUIDFromFormat(FormatIn), BitmapCreateCacheOption, WICBitmap.GetPP());
		if (Win32::Failed(Result))
		{
			LogError(Str("IWICImagingFactory::CreateBitmap Failed: {}"), ResultStr(Result));
		}
	}

	TTuple<const byte_t *, uint32_t> FD2DBitmap::Lock()
	{
		HRESULT Result = WICBitmap->Lock(nullptr, WICBitmapLockRead, WICBitmapLock.GetPP());
		if (Win32::Failed(Result))
		{
			LogError(Str("IWICBitmap::Lock Failed: {}"), ResultStr(Result));
			return { nullptr, 0 };
		}

		uint32_t DataSize = 0;
		WICInProcPointer InProcPointer = nullptr;
		WICBitmapLock->GetDataPointer(&DataSize, &InProcPointer);

		uint32_t RowPitch = 0;
		Result = WICBitmapLock->GetStride(&RowPitch);
		if (Win32::Failed(Result))
		{
			LogError(Str("IWICBitmap::GetStride Failed: {}"), ResultStr(Result));
			return { nullptr, 0 };
		}

		return { InProcPointer, RowPitch };
	}
	
	void FD2DBitmap::UnLock()
	{
		return WICBitmapLock.Reset();
	}
}
