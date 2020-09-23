#pragma once

#include "UIInc.h"
#include "Element.h"

namespace XE::UI
{
	class FControlTemplate : public FObject
	{
	public:
		FControlTemplate() = default;

		TReferPtr<UElement> LoadContent();

		const IDataProvider * DataProvider = nullptr;

	public:
		FType TargetType;

	public:
		static FProperty TargetTypeProperty;

	public:
		friend UI_API void Reflection(TNativeType<FControlTemplate> & NativeType);
	};
}

namespace XE
{
	template<>
	class UI_API TDataConverter<UI::FControlTemplate> : public TRefer<IDataConverter>
	{
	public:
		FVariant Convert(const IDataProvider * ValueProvider) const;
	};
}
