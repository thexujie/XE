#pragma once

#include "UIInc.h"
#include "Element.h"

namespace XE::UI
{
	class UI_API UTextBlock : public UElement
	{
	public:
		UTextBlock() = default;

		void OnRender(FGraphics & Graphics) const override;

	public:
		SizeF Measure(SizeF ConstrainedSize) const override;
		
		FString Text;
		FTextFormat TextFormat;

	public:
		static FProperty TextProperty;
		friend UI_API void Reflection(TNativeType<UTextBlock> & NativeType);
	};
}
