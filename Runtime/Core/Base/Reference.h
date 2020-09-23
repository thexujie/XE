#pragma once

namespace XE
{
	template<typename T>
	class TReference
	{
	public:
		TReference() : Value(*static_cast<T*>(nullptr)) {}
		TReference(T & Value_) : Value(Value_) {}
		TReference(const TReference &) = default;
		auto operator <=> (const TReference & That) const = default;
		bool operator == (const TReference & That) const = default;
		
		TReference & operator = (const TReference & That)
		{
			Memcpy(this, &That, sizeof(TReference<T>));
			return *this;
		}
		
		T & Value;
	};

	template<typename T>
	TReference<T> Ref(T & Value)
	{
		return TReference(Value);
	}

	template<typename T>
	inline constexpr bool IsRefV = false;

	template<typename T>
	inline constexpr bool IsRefV<TReference<T>> = true;

	template<typename T>
	concept RefAble = IsRefV<T>;

	template<typename T>
	struct IsRef : BoolConstant<false> {};

	template<typename T>
	struct IsRef<TReference<T>> : BoolConstant<true>
	{
		using Type = T;
	};
}