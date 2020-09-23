#pragma once

#include "GraphicsInc.h"

namespace XE
{
	class GRAPHICS_API FImage
	{
	public:
		FImage() = default;
		~FImage();
		FImage(FImage && That) noexcept : Format(That.Format), Bits(That.Bits), RowPitch(That.RowPitch), RowCount(That.RowCount), Size(That.Size)
		{
			That.Format = EFormat::None;
			That.Bits = nullptr;
			That.RowPitch = 0;
			That.RowCount = 0;
			That.Size = SizeU(0, 0);
		}
		FImage(const SizeU & SizeIn, EFormat FormatIn = EFormat::B8G8R8A8);

	public:
		EFormat GetFormat() const { return Format; }
		byte_t * GetBits() { return Bits; }
		const byte_t * GetBits() const { return Bits; }
		uint32_t GetRowPitch() const { return RowPitch; }
		uint32_t GetRowCount() const { return RowCount; }
		SizeU GetSize() const { return Size; }

		byte_t * GetRowBits(uint32_t RowIndex) { return Bits + RowPitch * RowIndex; }
		const byte_t * GetRowBits(uint32_t RowIndex) const { return Bits + RowPitch * RowIndex; }
		
	private:
		EFormat Format = EFormat::None;
		byte_t * Bits = nullptr;
		uint32_t RowPitch = 0;
		uint32_t RowCount = 0;
		SizeU Size = SizeU(0, 0);
	};
}
