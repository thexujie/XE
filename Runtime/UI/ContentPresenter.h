#pragma once

#include "UIInc.h"
#include "Element.h"

namespace XE::UI
{
	class UI_API UContentPresenter : public UElement
	{
	public:
		UContentPresenter() = default;

		 void InitializeUI() override;
		SizeF Measure(SizeF ConstrainedSize) const override;
		void Arrange(SizeF ArrangedSize) override;
		
	private:
		FVariant Content;

		TReferPtr<UElement> Element;
	};
}
