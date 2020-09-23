#pragma once

#include "UIInc.h"

namespace XE::UI
{
	class UI_API FStyle : public FObject
	{
	public:
		FStyle() = default;

		FType GetTargetType() const { return TargetType; }
		FStringView GetName() const { return Name; }

	public:
		FType TargetType;
		FString Name;

		TMap<FProperty, FVariant> Properties;
		FResourceDictionary Resources;

	public:
		static FProperty TargetTypeProperty;
		friend UI_API void Reflection(TNativeType<FStyle> & NativeType);
	};
}

namespace XE
{
	template<>
	class UI_API TDataConverter<UI::FStyle> : public TRefer<IDataConverter>
	{
	public:
		FVariant Convert(const IDataProvider * ValueProvider) const;
	};
}