#pragma once

#include "Types.h"

namespace XE
{
	template<typename T>
	class TFraction
	{
	public:
		T Num = 0;
		T Den = 1;

	public:
		constexpr TFraction() = default;
		constexpr TFraction(const TFraction &) = default;
		TFraction & operator =(const TFraction &) = default;
		auto operator<=>(const TFraction &) const noexcept = default;
		
#pragma warning (disable: 4056)
		constexpr TFraction(float Value) : Num(T(Value * 1000)), Den(1000) {}
#pragma warning (default : 4056)
		operator float() const { return static_cast<float>(Num) / Den; }

		uintx_t Hash() const { return HashJoin(Num, Den); }
	};

	template CORE_API class TFraction<int32_t>;
	using Fraction = TFraction<int32_t>;
}
