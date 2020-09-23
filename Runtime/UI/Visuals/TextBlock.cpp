#include "PCH.h"
#include "TextBlock.h"

namespace XE::UI
{
	FProperty UTextBlock::TextProperty(Str("Text"), &UTextBlock::Text);

	void UTextBlock::OnRender(FGraphics & Graphics) const
	{
		Graphics.DrawString(Text, RenderRect, TextFormat, Brushs::Red);
	}

	SizeF UTextBlock::Measure(SizeF ConstrainedSize) const
	{
		FFormattedText FormatedText(Text, TextFormat);
		return FormatedText.Size;
	}

	void Reflection(TNativeType<UTextBlock> & NativeType)
	{
		NativeType.RegisterProperty(UTextBlock::TextProperty);
	}
}
