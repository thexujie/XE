#include "PCH.h"
#include "FormattedText.h"
#include "Graphics.h"

namespace XE
{
	FFormattedText::FFormattedText(FStringView Text, const FTextFormat & TextFormat, const FCulture & Culture)
	{
		Load(Text, TextFormat, Culture);
	}
	
	void FFormattedText::Load(FStringView Text, const FTextFormat & TextFormat, const FCulture & Culture)
	{
		FGraphicsService * GraphicsService = GApplication->GetService<FGraphicsService>();
		if (!GraphicsService)
		{
			LogWarning(Str("No GraphicsService"));
			return;
		}

		TextLayout = GraphicsService->CreateTextLayout(Text, TextFormat, Culture);
		STextLayoutMetrics TextLayoutMetrics = TextLayout->GetMetrics();
		Size = TextLayoutMetrics.Size;
	}
}
