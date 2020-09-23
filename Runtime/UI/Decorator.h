#pragma once

#include "UIInc.h"
#include "Element.h"

namespace XE::UI
{
	class UI_API UDecorator : public UElement
	{
	public:
		UDecorator() = default;

		void SetChild(TReferPtr<UElement> Child_)
		{
			Child = Child_;
		}

	public:
		void InitializeUI() override;
		
	public:
		TReferPtr<UElement> Child;

	public:
		static FProperty ChildProperty;
		friend UI_API void Reflection(TNativeType<UDecorator> & NativeType);
	};
}
