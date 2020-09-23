#pragma once

#include "D2DInc.h"

namespace XE::Direct2D
{
	class DIRECT2D_API FD2DBitmap : public FPixmap
	{
	public:
		FD2DBitmap(FD2DDevice & D2DDeviceIn, const SizeU & SizeIn, EFormat FormatIn = EFormat::BGRA);
		~FD2DBitmap() = default;

		const SizeU & GetSize() const override { return Size; }
		TTuple<const byte_t *, uint32_t> Lock() override;
		void UnLock() override;
	public:
		IWICBitmap * GetWICBitmap() const { return WICBitmap.Get(); }
		
	private:
		FD2DDevice & D2DDevice;
		SizeU Size;
		EFormat Format = EFormat::None;

		TReferPtr<IWICBitmap> WICBitmap;
		TReferPtr<IWICBitmapLock> WICBitmapLock;
	};
}

