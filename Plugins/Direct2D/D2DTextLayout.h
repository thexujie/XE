#pragma once

#include "D2DInc.h"

namespace XE::Direct2D
{
	class FD2DTextLayout : public TRefer<ITextLayout>
	{
	public:
		FD2DTextLayout(FD2DDevice & D2DDevice_) : D2DDevice(D2DDevice_) {}
		void Load(FStringView Text, const FTextFormat & TextFormat, const FCulture & Culture);
		STextLayoutMetrics GetMetrics() const override;
		
	private:
		FD2DDevice & D2DDevice;
		TReferPtr<IDWriteTextLayout> DWriteTextLayout;
	};
}
