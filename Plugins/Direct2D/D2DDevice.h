#pragma once

#include "D2DInc.h"

namespace XE::Direct2D
{
	class DIRECT2D_API FD2DDevice : public FGraphicsService
	{
	public:
		FD2DDevice() = default;
		~FD2DDevice() = default;

		EError Initialize() override;
		void Finalize() override;

		IDWriteTextFormat * GetDWriteTextFormat(const FTextFormat & TextFormat);

		TVector<FPathPoint> StringToPath(FStringView String, const SizeF & LayoutSize, const FTextFormat & TextFormat) override;

	public:
		TReferPtr<ITextLayout> CreateTextLayout(FStringView Text, const FTextFormat & TextFormat, const FCulture & Culture) override;
		TSharedPtr<FPixmap> CreateBitmap(const SizeU & Size, EFormat Format = EFormat::BGRA) override;
		
		TSharedPtr<FGraphics> CreateGraphics(ptr_t WindowId) override;
		TSharedPtr<FGraphics> CreateGraphics(FPixmap & GrpBitmap) override;
		
	public:
		ID2D1Factory * GetD2D1Factory() const { return D2D1Factory.Get(); }
		IDWriteFactory3 * GetDWriteFactory3() const { return DWriteFactory3.Get(); }
		IWICImagingFactory * GetWICImagingFactory() const { return WICImagingFactory.Get(); }
		
	public:
		TReferPtr<ID2D1Factory> D2D1Factory;
		TReferPtr<IDWriteFactory3> DWriteFactory3;
		TReferPtr<IWICImagingFactory> WICImagingFactory;

	private:
		TMap<FTextFormat, TReferPtr<IDWriteTextFormat>> DWriteTextFormats;
	};
}
