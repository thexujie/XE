#include "PCH.h"
#include "D2DTextLayout.h"
#include "D2DDevice.h"

namespace XE::Direct2D
{
	void FD2DTextLayout::Load(FStringView Text, const FTextFormat & TextFormat, const FCulture & Culture)
	{
		IDWriteTextFormat * DWriteTextFormat = D2DDevice.GetDWriteTextFormat(TextFormat);
		if (!DWriteTextFormat)
			return;

		FWString TextW = Strings::ToWide(Text);
		HRESULT Result = D2DDevice.DWriteFactory3->CreateTextLayout(TextW.Data, (uint32_t)TextW.Size, DWriteTextFormat, Float32Max, Float32Max, DWriteTextLayout.GetPP());
		if (Win32::Failed(Result))
		{
			LogWarning(Str("CreateTextLayout Failed: {}"), ResultStr(Result));
			return;
		}
	}
	
	STextLayoutMetrics FD2DTextLayout::GetMetrics() const
	{
		DWRITE_TEXT_METRICS DwriteTextMetrics = {};
		DWriteTextLayout->GetMetrics(&DwriteTextMetrics);

		STextLayoutMetrics TextLayoutMetrics = {};
		TextLayoutMetrics.Size = { DwriteTextMetrics.widthIncludingTrailingWhitespace, DwriteTextMetrics.height };
		TextLayoutMetrics.TrimmedWidth = DwriteTextMetrics.width;
		return TextLayoutMetrics;
	}
}