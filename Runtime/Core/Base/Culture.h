#pragma once

#include "Types.h"
#include "String.h"

namespace XE
{
	class CORE_API FCulture
	{
	public:
		FCulture() = default;
		FCulture(FStringView Name_) : Name(Name_) {}
		auto operator <=>(const FCulture &) const = default;
		bool operator ==(const FCulture &) const = default;
		
		FString Name;
	};

	namespace Cultures
	{
		inline FCulture EN_US(Str("en-us"));
		inline FCulture ZH_CN(Str("zh-cn"));
	}
}
