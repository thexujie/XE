#pragma once

#include "CoreInc.h"

namespace XE
{
	enum class ERegexFlags
	{
		None = 0,
		IgnoreCase
	};
	template struct TFlag<ERegexFlags>;

	class CORE_API FRegex
	{
	public:
		FRegex(FStringView Expression_, ERegexFlags RegexFlags_ = ERegexFlags::None) : Expression(Expression_), RegexFlags(RegexFlags_) {}

		bool Match(FStringView String);
		bool Search(FStringView String);
		FStringView operator[](size_t Index) const { return Result[Index]; }
		
		FString Expression;
		ERegexFlags RegexFlags = ERegexFlags::None;
		TVector<FStringView> Result;
	};
}
