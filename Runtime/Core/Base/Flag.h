#pragma once

#include "Types.h"

namespace XE
{
	template <typename T, typename = void>
	struct TFlagHelper
	{
		using UnderlyingT = T;
	};

	template <typename T>
	struct TFlagHelper<T, EnableIfT<IsEnumV<T>>>
	{
		using UnderlyingT = typename std::underlying_type<T>::type;
	};

	template<typename Enum>
	struct TFlag
	{
		using UnderlyingT = typename TFlagHelper<Enum>::UnderlyingT;
		
		constexpr TFlag() = default;
		constexpr TFlag(const TFlag &) = default;
		constexpr TFlag & operator=(const TFlag &) = default;
		auto operator <=>(const TFlag &) const noexcept = default;
		
		constexpr TFlag(NullPtrT) : Value(static_cast<Enum>(0)) {}
		template<typename T, typename = EnableIfT<IsConvertibleToV<T, UnderlyingT> || IsSameV<T, Enum>>>
		constexpr TFlag(T ValueIn) : Value(static_cast<Enum>(static_cast<UnderlyingT>(ValueIn))) {}

		constexpr TFlag & operator = (Enum ValueIn)
		{
			Value = ValueIn;
			return *this;
		}

		TFlag & Set(Enum ValueIn, bool Set)
		{
			if (Set)
				Value = static_cast<Enum>(static_cast<UnderlyingT>(Value) | static_cast<UnderlyingT>(ValueIn));
			else
				Value = static_cast<Enum>(static_cast<UnderlyingT>(Value) & (~static_cast<UnderlyingT>(ValueIn)));
			return *this;
		}

		TFlag & Clear()
		{
			Value = static_cast<Enum>(0);
			return *this;
		}

		TFlag & Clear(Enum Components)
		{
			Value = static_cast<Enum>(static_cast<UnderlyingT>(Value) & (~static_cast<UnderlyingT>(Components)));
			return *this;
		}

		template<typename T, typename = EnableIfT<IsConvertibleToV<T, UnderlyingT> || IsSameV<T, Enum>>>
		bool Any(const T & Component) const
		{
			return (static_cast<UnderlyingT>(Value) & static_cast<UnderlyingT>(Component)) != 0;
		}

		template<typename T, typename = EnableIfT<IsConvertibleToV<T, UnderlyingT> || IsSameV<T, Enum>>>
		bool All(const T & Component) const
		{
			return (static_cast<UnderlyingT>(Value) & static_cast<UnderlyingT>(Component)) == static_cast<UnderlyingT>(Component);
		}

		TFlag operator !() const
		{
			return static_cast<Enum>(!static_cast<UnderlyingT>(Value));
		}

		TFlag operator ~() const
		{
			return static_cast<Enum>(~static_cast<UnderlyingT>(Value));
		}

		template<typename T, typename = EnableIfT<IsConvertibleToV<T, UnderlyingT> || IsSameV<T, Enum>>>
		TFlag operator | (const T & Component) const
		{
			return static_cast<Enum>(static_cast<UnderlyingT>(Value) | static_cast<UnderlyingT>(Component));
		}

		template<typename T, typename = EnableIfT<IsConvertibleToV<T, UnderlyingT> || IsSameV<T, Enum>>>
		TFlag operator & (const T & Component) const
		{
			return static_cast<Enum>(static_cast<UnderlyingT>(Value) & static_cast<UnderlyingT>(Component));
		}

		template<typename T, typename = EnableIfT<IsConvertibleToV<T, UnderlyingT> || IsSameV<T, Enum>>>
		TFlag & operator |= (const T & Component)
		{
			Value = static_cast<Enum>(static_cast<UnderlyingT>(Value) | static_cast<UnderlyingT>(Component));
			return *this;
		}
		
		template<typename T, typename = EnableIfT<IsConvertibleToV<T, UnderlyingT> || IsSameV<T, Enum>>>
		TFlag & operator &= (const T & Component)
		{
			Value = static_cast<Enum>(static_cast<UnderlyingT>(Value) & static_cast<UnderlyingT>(Component));
			return *this;
		}

		template<typename T, typename = EnableIfT<IsConvertibleToV<T, UnderlyingT> || IsSameV<T, Enum>>>
		TFlag operator == (const T & Component) const { return static_cast<UnderlyingT>(Value) == static_cast<UnderlyingT>(Component); }

		template<typename T, typename = EnableIfT<IsConvertibleToV<T, UnderlyingT> || IsSameV<T, Enum>>>
		TFlag operator != (const T & Component) const { return static_cast<UnderlyingT>(Value) != static_cast<UnderlyingT>(Component); }

		operator UnderlyingT() const
		{
			return static_cast<UnderlyingT>(Value);
		}

		const Enum & Get() const { return Value; }
		Enum & Get() { return Value; }

		operator Enum & () { return Value; }
		operator const Enum & () const { return Value; }
		
	public:
		Enum Value = static_cast<Enum>(0);
	};

	template<typename Enum>
	constexpr bool FlagEnabledValue = IsCompleteV<TFlag<Enum>> && IsEnumV<Enum> && !IsConvertibleToV<Enum, int>;

	template<typename Enum>
	concept FlagEnabledEnum = IsCompleteV<TFlag<Enum>> && IsEnumV<Enum> && !IsConvertibleToV<Enum, int>;
	
	template<typename T>
	struct THash<TFlag<T>>
	{
		uintx_t operator()(const TFlag<T> & Flag) const noexcept
		{
			return THash<T>()(Flag.Get());
		}
	};

	template<XE::FlagEnabledEnum Enum>
	constexpr bool operator *(const Enum & Lhs, const Enum & Rhs)
	{
		using UnderlyingType = XE::UnderlyingTypeT<Enum>;
		return (static_cast<UnderlyingType>(Lhs) & static_cast<UnderlyingType>(Rhs)) != 0;
	}
}


template<XE::FlagEnabledEnum Enum>
constexpr Enum operator |(const Enum & Lhs, const Enum & Rhs)
{
	using UnderlyingType = XE::UnderlyingTypeT<Enum>;
	return static_cast<Enum>(static_cast<UnderlyingType>(Lhs) | static_cast<UnderlyingType>(Rhs));
}

template<XE::FlagEnabledEnum Enum>
constexpr Enum operator &(const Enum & Lhs, const Enum & Rhs)
{
	using UnderlyingType = XE::UnderlyingTypeT<Enum>;
	return static_cast<Enum>(static_cast<UnderlyingType>(Lhs) & static_cast<UnderlyingType>(Rhs));
}

template<XE::FlagEnabledEnum Enum>
constexpr Enum & operator |=(Enum & Lhs, const Enum & Rhs)
{
	using underlying = typename std::underlying_type<Enum>::type;
	return Lhs = static_cast<Enum>(static_cast<underlying>(Lhs) | static_cast<underlying>(Rhs));
}

template<XE::FlagEnabledEnum Enum>
constexpr Enum & operator &=(Enum & Lhs, const Enum & Rhs)
{
	using UnderlyingType = XE::UnderlyingTypeT<Enum>;
	return Lhs = static_cast<Enum>(static_cast<UnderlyingType>(Lhs) & static_cast<UnderlyingType>(Rhs));
}


template<XE::FlagEnabledEnum Enum>
Enum FlagSelect(const Enum & Value, bool Set)
{
	return Set ? Value : static_cast<Enum>(0);
}

template<XE::FlagEnabledEnum Enum>
bool operator !(const Enum & Value)
{
	using UnderlyingType = XE::UnderlyingTypeT<Enum>;
	return static_cast<UnderlyingType>(Value) == 0;
}


template<typename Enum>
XE::EnableIfT<XE::FlagEnabledValue<Enum>, bool> operator !(const Enum & Lhs)
{
	using underlying = typename std::underlying_type<Enum>::type;
	return static_cast<underlying>(Lhs) == 0;
}

template<typename Enum>
constexpr XE::EnableIfT<XE::FlagEnabledValue<Enum>, Enum> operator ~(const Enum & Lhs)
{
	using underlying = typename std::underlying_type<Enum>::type;
	return static_cast<Enum>(~static_cast<underlying>(Lhs));
}
