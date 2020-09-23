#pragma once

#include "StringView.h"
#include "fmt/format.h"
#include "fmt/os.h"

namespace XE
{
	template<typename T>
	class TFormatter;

	template<typename T>
	concept TFormatAble = requires(const T & Instance, FStringView Formal)
	{
		{ TFormatter<T>().Format(Instance, Formal) } -> ConvertibleTo<FStringView>;
	};
	template<typename T>
	inline constexpr bool IsOutterFormatAbleV = false;

	template<TFormatAble T>
	inline constexpr bool IsOutterFormatAbleV<T> = true;

	template<typename T>
	concept FunctionFormatAble = requires(const T & Instance, FStringView Formal)
	{
		Instance.Format(Formal);
	};
	template<typename T>
	inline constexpr bool IsInnerFormatAble = false;
	template<FunctionFormatAble T>
	inline constexpr bool IsInnerFormatAble<T> = true;

	template<typename T>
	inline constexpr bool IsFormatAbleV = false;

	template<TFormatAble T>
	inline constexpr bool IsFormatAbleV<T> = true;

	template<FunctionFormatAble T>
	inline constexpr bool IsFormatAbleV<T> = true;

	template<typename T>
	inline constexpr bool IsSystemFormatAbleV = IsIntegralV<T> || IsFloatingPointV<T> ||
		IsAnyOfV<T,
		void *, const void *,
		fmt::detail::utf16_to_utf8, fmt::monostate
		>;
	template<Character T>
	inline constexpr bool IsSystemFormatAbleV<T *> = true;
	
	template<Character T>
	inline constexpr bool IsSystemFormatAbleV<const T *> = true;
	
	template<Character T, size_t Length>
	inline constexpr bool IsSystemFormatAbleV<T[Length]> = true;

	template<Character T>
	inline constexpr bool IsSystemFormatAbleV<TView<T>> = true;

	template<Character T>
	inline constexpr bool IsSystemFormatAbleV<std::basic_string_view<T>> = true;

	template<Character T>
	inline constexpr bool IsSystemFormatAbleV<fmt::basic_string_view<T>> = true;

	template<typename T>
	inline constexpr bool IsCustomFormatAbleV = !IsSystemFormatAbleV<T>;

	template<typename T>
	concept CustomFormatAble = IsCustomFormatAbleV<T>;

	template<typename T>
	inline constexpr bool IsLibFormatAbleV = IsSystemFormatAbleV<T> || IsOutterFormatAbleV<T> || IsInnerFormatAble<T>;

	template<typename T> struct IsLibFormatAble : BoolConstant<IsLibFormatAbleV<T>> {};


	template<typename CharT>
	fmt::basic_memory_buffer<CharT> & GetThreadLocalFormatBuffer();

	template CORE_API fmt::basic_memory_buffer<char> & GetThreadLocalFormatBuffer<char>();
	template CORE_API fmt::basic_memory_buffer<char8_t> & GetThreadLocalFormatBuffer<char8_t>();
	template CORE_API fmt::basic_memory_buffer<wchar_t> & GetThreadLocalFormatBuffer<wchar_t>();

	template<typename T>
	void CheckLibFormatAble()
	{
		static_assert(IsLibFormatAbleV<T>, __FUNCTION__ " Custom type need a TFormatter<T> or ToString() member.");
	};

	template<typename T = void, typename ...ArgsT>
	void CheckLibFormat();

	template<typename T, typename ...ArgsT>
	void CheckLibFormat()
	{
		CheckLibFormatAble<T>();
		CheckLibFormat<ArgsT...>();
	}

	template<>
	inline void CheckLibFormat<>()
	{
	}

	template<typename CharT, typename ...ArgsT>
	TView<CharT> FormatImpl(TView<CharT> Formal, ArgsT && ...Args)
	{
		CheckLibFormat<RemoveCVRefT<ArgsT>...>();

		if constexpr (ConjunctionV<IsLibFormatAble<RemoveCVRefT<ArgsT>>...>)
		{
			fmt::basic_memory_buffer<CharT> & FormatBuffer = GetThreadLocalFormatBuffer<CharT>();
			fmt::format_to(FormatBuffer, fmt::basic_string_view<CharT>(Formal.Data, Formal.Size), Forward<ArgsT>(Args)...);
			return TView<CharT>(FormatBuffer.begin(), FormatBuffer.size());
		}
		else
			return Formal;
	}

	template<typename ...ArgsT>
	TView<char> Format(TView<char> Formal, ArgsT && ...Args)
	{
		return FormatImpl(Formal, Forward<ArgsT>(Args)...);
	}

	template<typename ...ArgsT>
	TView<char8_t> Format(TView<char8_t> Formal, ArgsT && ...Args)
	{
		return FormatImpl(Formal, Forward<ArgsT>(Args)...);
	}

	template<typename ...ArgsT>
	TView<wchar_t> Format(TView<wchar_t> Formal, ArgsT && ...Args)
	{
		return FormatImpl(Formal, Forward<ArgsT>(Args)...);
	}

	template<typename T>
	FStringView FormatValue(const T & Value, FStringView Formal = FStringView::Empty) { return TFormatter<T>().Format(Value, Formal); }

	template<typename T>
	auto Parse(FStringView String) { return TFormatter<T>().Parse(String); }
	
	template<Integral T>
	class TFormatter<T>
	{
	public:
		FStringView Format(const T & Value, FStringView Formal = StrView("{}")) const
		{
			if constexpr (IsCharacterV<T>)
				return XE::Format(Formal, Char(Value));
			else
				return XE::Format(Formal, Value);
		}

		T Parse(FStringView String, int32_t Radis = 0) const
		{
			if constexpr (IsCharacterV<T>)
				return T(String[0]);
			else
				return ToInterger<Char, T>(String.Data, String.Size, Radis);
		}
	};

	template<FloatingPoint T>
	class TFormatter<T>
	{
	public:
		FStringView Format(const T & Value, FStringView Formal = StrView("{}")) const
		{
			return XE::Format(Formal, Value);
		}

		T Parse(FStringView String) const
		{
			return ToFloatingPoint<Char, T>(String.Data, String.Size);
		}
	};

	template<>
	class TFormatter<FStringView>
	{
	public:
		FStringView Format(const FStringView & Value, FStringView Formal = StrView("{}")) const
		{
			return Value;
		}

		FStringView Parse(FStringView String) const
		{
			return String;
		}
	};

	template<Enum T>
	class TFormatter<T>
	{
	public:
		FStringView Format(const T & Value, FStringView Formal = FStringView::Empty) const
		{
			return XE::Format(StrView("{}"), UnderlyingTypeT<T>(Value));
		}

		T Parse(FStringView String) const
		{
			throw EError::NotImplemented;
		}
	};
}

namespace fmt
{
	template<XE::Character CharT>
	struct formatter<XE::TView<CharT>, CharT> : formatter<basic_string_view<CharT>, CharT>
	{
		template <typename FormatContext>
		auto format(XE::TView<CharT> String, FormatContext & Context)
		{
			return formatter<basic_string_view<CharT>, CharT>::format(basic_string_view<CharT>(String.Data, String.Size), Context);
		}
	};

	template<XE::CustomFormatAble T>
	struct formatter<T, XE::Char> : formatter<basic_string_view<XE::Char>, XE::Char>
	{
		static_assert(XE::IsCustomFormatAbleV<T>, "Custom type need a TFormatter<T> or ToString() member.");
		static_assert(XE::IsOutterFormatAbleV<T> || XE::IsInnerFormatAble<T>, "Custom type need a TFormatter<T> or ToString() member.");

		XE::FStringView Formal;
		constexpr auto parse(fmt::basic_format_parse_context<XE::Char> & Context)
		{
			auto Iter = Context.begin(), IterEnd = Context.end();
			while (Iter != IterEnd && *Iter != Str('}'))
				Iter++;

			Formal = XE::FStringView(Context.begin(), Iter - Context.begin());
			return Iter;
		}

		template <typename FormatContext>
		auto format(const T & Value, FormatContext & Context)
		{
			if constexpr (XE::IsOutterFormatAbleV<T>)
				return formatter<XE::FStringView, XE::Char>().format(XE::TFormatter<T>().Format(Value, Formal), Context);
			else if constexpr (XE::IsInnerFormatAble<T>)
				return formatter<XE::FStringView, XE::Char>().format(Value.Format(Formal), Context);
			else
				static_assert(XE::IsCustomFormatAbleV<T>, "Custom type need a TFormatter<T> or ToString() member.");
		}
	};
}
