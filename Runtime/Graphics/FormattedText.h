#pragma once

#include "GraphicsInc.h"
#include "TextLayout.h"
#include "Typeface.h"

namespace XE
{
	enum class ETextAlignment
	{
		Left = 0,
		Right,
		Center,
		Justify,
	};

	enum class ETextTrimming
	{
		None = 0,
		Character,
		Word,
	};
	
	class GRAPHICS_API FFormattedText : public FObject
	{
	public:
		FFormattedText() = default;
		FFormattedText(FStringView Text, const FTextFormat & TextFormat, const FCulture & Culture = FCulture());

		void Load(FStringView Text, const FTextFormat & TextFormat, const FCulture & Culture = FCulture());
		
	public:
		SizeF Size;
		TReferPtr<ITextLayout> TextLayout;
	};
}
