#pragma once

#include "Hash.h"

namespace std
{
	using u8ios = basic_ios<char8_t, char_traits<char8_t>>;
	using u8streambuf = basic_streambuf<char8_t, char_traits<char8_t>>;
	using u8istream = basic_istream<char8_t, char_traits<char8_t>>;
	using u8ostream = basic_ostream<char8_t, char_traits<char8_t>>;
	using u8iostream = basic_iostream<char8_t, char_traits<char8_t>>;
	using u8stringbuf = basic_stringbuf<char8_t, char_traits<char8_t>, allocator<char8_t>>;
	using u8istringstream = basic_istringstream<char8_t, char_traits<char8_t>, allocator<char8_t>>;
	using u8ostringstream = basic_ostringstream<char8_t, char_traits<char8_t>, allocator<char8_t>>;
	using u8stringstream = basic_stringstream<char8_t, char_traits<char8_t>, allocator<char8_t>>;
	using u8filebuf = basic_filebuf<char8_t, char_traits<char8_t>>;
	using u8ifstream = basic_ifstream<char8_t, char_traits<char8_t>>;
	using u8ofstream = basic_ofstream<char8_t, char_traits<char8_t>>;
	using u8fstream = basic_fstream<char8_t, char_traits<char8_t>>;

	using xstring = basic_string<XE::Char, char_traits<XE::Char>>;
	using xstring_view = basic_string_view<XE::Char, char_traits<XE::Char>>;
	using xios = basic_ios<XE::Char, char_traits<XE::Char>>;
	using xstreambuf = basic_streambuf<XE::Char, char_traits<XE::Char>>;
	using xistream = basic_istream<XE::Char, char_traits<XE::Char>>;
	using xostream = basic_ostream<XE::Char, char_traits<XE::Char>>;
	using xiostream = basic_iostream<XE::Char, char_traits<XE::Char>>;
	using xstringbuf = basic_stringbuf<XE::Char, char_traits<XE::Char>, allocator<XE::Char>>;
	using xistringstream = basic_istringstream<XE::Char, char_traits<XE::Char>, allocator<XE::Char>>;
	using xostringstream = basic_ostringstream<XE::Char, char_traits<XE::Char>, allocator<XE::Char>>;
	using xstringstream = basic_stringstream<XE::Char, char_traits<XE::Char>, allocator<XE::Char>>;
	using xfilebuf = basic_filebuf<XE::Char, char_traits<XE::Char>>;
	using xifstream = basic_ifstream<XE::Char, char_traits<XE::Char>>;
	using xofstream = basic_ofstream<XE::Char, char_traits<XE::Char>>;
	using xfstream = basic_fstream<XE::Char, char_traits<XE::Char>>;
}

namespace XE
{
	namespace Strings
	{
		template<typename CharT>
		TView<CharT> WhiteSpaces;

		template<>
		inline TView<char> WhiteSpaces<char> = " \t\n\r\0\x0b";
		template<>
		inline TView<wchar_t> WhiteSpaces<wchar_t> = L" \t\n\r\0\x0b";
		template<>
		inline TView<char8_t> WhiteSpaces<char8_t> = u8" \t\n\r\0\x0b";
		
		template<typename CharT>
		bool IsWhiteSpace(CharT Ch)
		{
			return Contains(WhiteSpaces<CharT>, Ch);
		}

		template<typename CharT>
		CharT ToUpper(CharT Ch)
		{
			return ('a' <= Ch && Ch <= 'z') ? Ch - 32 : Ch;
		}

		template<typename CharT>
		CharT ToLower(CharT Ch)
		{
			return ('A' <= Ch && Ch <= 'A') ? Ch + 32 : Ch;
		}
		
		template<typename CharT>
		int32_t Compare(const CharT * StringLeft, size_t LengthLeft, const CharT * StringRight, size_t LengthRight)
		{
			if (LengthLeft == NullIndex) LengthLeft = Length<CharT>(StringLeft);
			if (LengthRight == NullIndex) LengthRight = Length<CharT>(StringRight);

			if (!LengthLeft && !LengthRight)
				return 0;

			if (!LengthLeft) return LengthRight ? 1 : -1;
			if (!LengthRight) return LengthLeft ? 1 : -1;


			if constexpr (IsAnyOfV<CharT, char, char8_t>)
				return strncmp((const char *)StringLeft, (const char *)StringRight, Max(LengthLeft, LengthRight));
			else if constexpr (IsAnyOfV<CharT, wchar_t, char16_t>)
				return wcsncmp((const wchar_t *)StringLeft, (const wchar_t *)StringRight, Max(LengthLeft, LengthRight));
			else
				throw EError::NotImplemented;
		}

		template<typename CharT>
		int32_t ICCompare(const CharT * StringLeft, size_t LengthLeft, const CharT * StringRight, size_t LengthRight)
		{
			if (LengthLeft == NullIndex) LengthLeft = Length<CharT>(StringLeft);
			if (LengthRight == NullIndex) LengthRight = Length<CharT>(StringRight);

			if (!LengthLeft && !LengthRight)
				return 0;

			if (!LengthLeft) return LengthRight ? 1 : -1;
			if (!LengthRight) return LengthLeft ? 1 : -1;


			if constexpr (IsAnyOfV<CharT, char, char8_t>)
				return _strnicmp((const char *)StringLeft, (const char *)StringRight, Max(LengthLeft, LengthRight));
			else if constexpr (IsAnyOfV<CharT, wchar_t, char16_t>)
				return _wcsnicmp((const wchar_t *)StringLeft, (const wchar_t *)StringRight, Max(LengthLeft, LengthRight));
			else
				throw EError::NotImplemented;
		}
	}
	
	template<Character T>
	auto operator<=>(const TView<T> & Lhs, const TView<T> & Rhs)
	{
		return Strings::Compare<T>(Lhs.Data, Lhs.Size, Rhs.Data, Rhs.Size) <=> 0;
	}

	template<Character T>
	bool operator==(const TView<T> & Lhs, const TView<T> & Rhs)
	{
		return Strings::Compare<T>(Lhs.Data, Lhs.Size, Rhs.Data, Rhs.Size) == 0;
	}

	template<Character T, size_t Length>
	bool operator==(const TView<T> & Lhs, const T (&Rhs)[Length])
	{
		return Strings::Compare<T>(Lhs.Data, Lhs.Size, Rhs, Length > 1 ? Length - 1 : 0) == 0;
	}

	template<Character T, size_t Length>
	bool operator==(const T(&Lhs)[Length], const TView<T> & Rhs)
	{
		return Strings::Compare<T>(Lhs, Length > 1 ? Length - 1 : 0, Rhs.Data, Rhs.Size) == 0;
	}

	namespace Strings
	{
		template<typename CharT>
		int32_t Compare(TView<CharT> Left, TView<CharT> Right)
		{
			return Compare<CharT>(Left.Data, Left.Size, Right.Data, Right.Size);
		}

		template<typename CharT>
		int32_t ICCompare(TView<CharT> Left, TView<CharT> Right)
		{
			return ICCompare<CharT>(Left.Data, Left.Size, Right.Data, Right.Size);
		}
	}

	//template class CORE_API TView<char>;
	//template class CORE_API TView<wchar_t>;
	//template class CORE_API TView<char8_t>;
	//template class CORE_API TView<char16_t>;
	//template class CORE_API TView<char32_t>;
	//
	//using FStringView = TView<Char>;
	//template<> TView<char> TView<char>::Empty;
	//template<> TView<wchar_t> TView<wchar_t>::Empty;
	//template<> TView<char8_t> TView<char8_t>::Empty;
	//template<> TView<char16_t> TView<char16_t>::Empty;
	//template<> TView<char32_t> TView<char32_t>::Empty;

	//using FWStringView = CORE_API TView<wchar_t>;
	//using FU8StringView = CORE_API TView<char8_t>;
	//using FU16StringView = CORE_API TView<char16_t>;
	//using FU32StringView = CORE_API TView<char32_t>;
		
	using FAStringView = TView<char>;
	using FWStringView = TView<wchar_t>;
	using FU8StringView = TView<char8_t>;
	using FU16StringView = TView<char16_t>;
	using FU32StringView = TView<char32_t>;
	using FStringView = TView<Char>;

	inline XE::TView<char> operator ""_sv(const char * str, size_t size)
	{
		return XE::TView<char>(str, size);
	}

	inline XE::TView<char8_t> operator ""_sv(const char8_t * str, size_t size)
	{
		return XE::TView<char8_t>(str, size);
	}

	namespace Strings
	{
		CORE_API int32_t Stricmp(FAStringView StringLeft, FAStringView StringRight);
		CORE_API int32_t Stricmp(FU8StringView StringLeft, FU8StringView StringRight);
		CORE_API int32_t Stricmp(FU16StringView StringLeft, FU16StringView StringRight);
		CORE_API int32_t Stricmp(FU32StringView StringLeft, FU32StringView StringRight);
		CORE_API int32_t Stricmp(FWStringView StringLeft, FWStringView StringRight);

		template<typename CharT>
		TEnumerable<TView<CharT>> Split(TView<CharT> String, const CharT & Delimiter)
		{
			size_t Offset = 0;
			while (true)
			{
				TView<CharT> Token = GetLine<CharT>(String, Offset, Delimiter);
				if (!Token.Size)
					break;
				co_yield Token;
			}
		}

		template<typename CharT>
		TEnumerable<TView<CharT>> Split(TView<CharT> String, TView<CharT> Delimiters)
		{
			size_t Offset = 0;
			while (true)
			{
				TView<CharT> Token = GetLine<CharT>(String, Offset, Delimiters);
				if (!Token.Size)
					break;
				co_yield Token;
			}
		}

		template<typename CharT>
		TView<CharT> Trim(TView<CharT> String)
		{
			for (size_t Index = 0; Index < String.Size; ++Index)
			{
				if (!Strings::IsWhiteSpace(String.Data[Index]))
				{
					String.Size -= Index;
					String.Data += Index;
					break;
				}
			}

			for (size_t Index = 0; Index < String.Size; ++Index)
			{
				if (!Strings::IsWhiteSpace(String.Data[String.Size - 1 - Index]))
				{
					String.Size -= Index;
					break;
				}
			}

			return String;
		}
	}
}
