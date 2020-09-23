#include "PCH.h"
#include "String.h"
#include "Atomic.h"

namespace XE
{
	static const size_t StringBufferCount = 128;
	static const int STRING_BUFFER_SIZE = 4;
	enum EFormatBuffer
	{
		EFormatBuffer_Ansi = 0,
		EFormatBuffer_Utf8,
		EFormatBuffer_Wide,
		EFormatBuffer_Count,
	};

	template<typename CharT>
	static int32_t GStringBufferIndex;

	template<typename CharT>
	static TString<CharT> GStringBuffers[StringBufferCount];

	template<typename CharT>
	TString<CharT> & GetThreadLocalStringBuffer()
	{
		int32_t Index = Atomics::IncFetch(GStringBufferIndex<CharT>) - 1;
		auto & ThreadLocalStringBuffer = GStringBuffers<CharT>[Index % StringBufferCount];
		if (ThreadLocalStringBuffer.Capacity < STRING_BUFFER_SIZE)
			ThreadLocalStringBuffer.Resize(0, STRING_BUFFER_SIZE);
		else
			ThreadLocalStringBuffer.Resize(0);
		return ThreadLocalStringBuffer;
	}

	template TString<char> & GetThreadLocalStringBuffer<char>();
	template TString<char8_t> & GetThreadLocalStringBuffer<char8_t>();
	template TString<wchar_t> & GetThreadLocalStringBuffer<wchar_t>();

	namespace Strings
	{
		template<typename CharT>
		UINT GetCodePage()
		{
			if constexpr (IsSameV<CharT, char>) return CP_ACP;
			if constexpr (IsSameV<CharT, char8_t>) return CP_UTF8;
			else return 0;
		}
		template<typename InT, typename OutT>
		TView<OutT> ConvertString(const InT * Source, size_t SourceLength)
		{
			if (SourceLength == NullPos)
				SourceLength = Length(Source);

			if (!SourceLength)
				return {};

			if constexpr (IsSameV<InT, OutT>)
				return TView<OutT>(Source, SourceLength);

			TView<wchar_t> WideString;
			if constexpr (IsSameV<InT, wchar_t>)
				WideString = { Source, SourceLength };
			else
			{
				TString<wchar_t> & ConvertBufferWide = GetThreadLocalStringBuffer<wchar_t>();
				UINT InputCodePage = GetCodePage<InT>();
				int NumWideChars = MultiByteToWideChar(InputCodePage, 0, reinterpret_cast<const char *>(Source), static_cast<int>(SourceLength), ConvertBufferWide.GetData(), (int)ConvertBufferWide.GetCapacityWithoutNull());
				if (NumWideChars == 0)
				{
					if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
						return {};

					NumWideChars = MultiByteToWideChar(InputCodePage, 0, reinterpret_cast<const char *>(Source), static_cast<int>(SourceLength), nullptr, 0);
					if (NumWideChars == 0)
						return {};

					ConvertBufferWide.Resize(NumWideChars);
					int NumWideChars2 = MultiByteToWideChar(InputCodePage, 0, reinterpret_cast<const char *>(Source), static_cast<int>(SourceLength), ConvertBufferWide.GetData(), (int)ConvertBufferWide.GetCapacityWithoutNull());
					assert(NumWideChars2 == NumWideChars);
				}
				else
					ConvertBufferWide.Resize(NumWideChars);
				WideString = ConvertBufferWide;
			}

			if constexpr (IsSameV<OutT, wchar_t>)
				return WideString;
			else
			{
				TString<OutT> & ConvertBufferOut = GetThreadLocalStringBuffer<OutT>();
				UINT OutputCodePage = GetCodePage<OutT>();
				int NumOutChars = WideCharToMultiByte(OutputCodePage, 0, WideString.Data, static_cast<int>(WideString.Size), reinterpret_cast<char *>(ConvertBufferOut.Data), (int)ConvertBufferOut.GetCapacityWithoutNull(), nullptr, nullptr);
				if (NumOutChars == 0)
				{
					if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
						return {};

					NumOutChars = WideCharToMultiByte(OutputCodePage, 0, WideString.Data, static_cast<int>(WideString.Size), nullptr, 0, nullptr, nullptr);
					if (NumOutChars == 0)
						return {};

					ConvertBufferOut.Resize(NumOutChars);
					int NumOutChars2 = WideCharToMultiByte(OutputCodePage, 0, WideString.Data, static_cast<int>(WideString.Size), reinterpret_cast<char *>(ConvertBufferOut.Data), (int)ConvertBufferOut.GetCapacityWithoutNull(), nullptr, nullptr);
					assert(NumOutChars2 == NumOutChars);
				}
				else
					ConvertBufferOut.Resize(NumOutChars);
				return ConvertBufferOut;
			}
		}

		FU8StringView AnsiToUtf8(const char * String, size_t StringLength)
		{
			return ConvertString<char, char8_t>(String, StringLength);
		}

		FWStringView AnsiToWide(const char * String, size_t StringLength)
		{
			return ConvertString<char, wchar_t>(String, StringLength);
		}

		FAStringView Utf8ToAnsi(const char8_t * String, size_t StringLength)
		{
			return ConvertString<char8_t, char>(String, StringLength);
		}

		FWStringView Utf8ToWide(const char8_t * String, size_t StringLength)
		{
			return ConvertString<char8_t, wchar_t>(String, StringLength);
		}

		FAStringView WideToAnsi(const wchar_t * String, size_t StringLength)
		{
			return ConvertString<wchar_t, char>(String, StringLength);
		}

		FU8StringView WideToUtf8(const wchar_t * String, size_t StringLength)
		{
			return ConvertString<wchar_t, char8_t>(String, StringLength);
		}

		FAStringView Printf(const char * Format, ...)
		{
			TString<char> & GConvertBufferAnsi = GetThreadLocalStringBuffer<char>();

			va_list ArgList;
			va_start(ArgList, Format);
			int32_t NumAnsiChars = std::vsnprintf(GConvertBufferAnsi.GetData(), GConvertBufferAnsi.GetCapacityWithoutNull(), Format, ArgList);
			// error happend
			if (NumAnsiChars == -1)
				return FAStringView();

			if (NumAnsiChars > GConvertBufferAnsi.GetCapacityWithoutNull())
			{
				GConvertBufferAnsi.Resize(0, NumAnsiChars + 1);
				size_t NumAnsiCharsExpect = NumAnsiChars;
				NumAnsiChars = std::vsnprintf(GConvertBufferAnsi.GetData(), GConvertBufferAnsi.GetCapacityWithoutNull(), Format, ArgList);
				assert(NumAnsiCharsExpect == NumAnsiChars);
			}

			return FAStringView(GConvertBufferAnsi.GetData(), NumAnsiChars);
		}

		FU8StringView Printf(const char8_t * Format, ...)
		{
			TString<char8_t> & ConvertBufferUtf8 = GetThreadLocalStringBuffer<char8_t>();

			va_list ArgList;
			va_start(ArgList, Format);
			int32_t NumUtf8Chars = std::vsnprintf((char *)ConvertBufferUtf8.GetData(), ConvertBufferUtf8.GetCapacityWithoutNull(), (const char *)Format, ArgList);
			// error happend
			if (NumUtf8Chars == -1)
				return FU8StringView();

			if (NumUtf8Chars > ConvertBufferUtf8.GetCapacityWithoutNull())
			{
				ConvertBufferUtf8.Resize(0, NumUtf8Chars + 1);
				size_t NumUtf8CharsExpect = NumUtf8Chars;
				NumUtf8Chars = std::vsnprintf((char *)ConvertBufferUtf8.GetData(), ConvertBufferUtf8.GetCapacityWithoutNull(), (const char *)Format, ArgList);
				assert(NumUtf8CharsExpect == NumUtf8Chars);
			}

			return FU8StringView(ConvertBufferUtf8.GetData(), NumUtf8Chars);
		}

		FWStringView Printf(const wchar_t * Format, ...)
		{
			TString<wchar_t> & ConvertBufferWide = GetThreadLocalStringBuffer<wchar_t>();

			va_list ArgList;
			va_start(ArgList, Format);
			int32_t NumWideChars = std::vswprintf(ConvertBufferWide.GetData(), ConvertBufferWide.GetCapacityWithoutNull(), Format, ArgList);
			// error happend
			if (NumWideChars == -1)
				return FWStringView();

			if (NumWideChars > ConvertBufferWide.GetCapacityWithoutNull())
			{
				ConvertBufferWide.Resize(0, NumWideChars + 1);
				size_t NumWideCharsExpect = NumWideChars;
				NumWideChars = std::vswprintf(ConvertBufferWide.GetData(), ConvertBufferWide.GetCapacityWithoutNull(), Format, ArgList);
				assert(NumWideCharsExpect == NumWideChars);
			}

			return FWStringView(ConvertBufferWide.GetData(), NumWideChars);
		}

		int32_t ToBool(FStringView String)
		{
			return TStringToBoolean(String.Data, String.Size);
		}

		int32_t ToInt32(FStringView String, uint32_t Radix)
		{
			return ToInterger<Char, int32_t>(String.Data, String.Size);
		}

		uint32_t ToUInt32(FStringView String, uint32_t Radix)
		{
			return ToInterger<Char, uint32_t>(String.Data, String.Size);
		}

		int64_t ToInt64(FStringView String, uint32_t Radix)
		{
			return ToInterger<Char, int32_t>(String.Data, String.Size);
		}

		uint64_t ToUInt64(FStringView String, uint32_t Radix)
		{
			return ToInterger<Char, uint64_t>(String.Data, String.Size);
		}

		intx_t ToIntX(FStringView String, uint32_t Radix)
		{
			return ToInterger<Char, intx_t>(String.Data, String.Size);
		}

		uintx_t ToUIntX(FStringView String, uint32_t Radix)
		{
			return ToInterger<Char, uintx_t>(String.Data, String.Size);
		}

		float32_t ToFloat32(FStringView String)
		{
			return ToFloatingPoint<Char, float32_t>(String.Data, String.Size);
		}

		float64_t ToFloat64(FStringView String)
		{
			return ToFloatingPoint<Char, float64_t>(String.Data, String.Size);
		}
	}

	template<typename CharT>
	TView<CharT> operator +(TView<CharT> Lhs, TView<CharT> Rhs)
	{
		TString<CharT> & Buffer = XE::GetThreadLocalStringBuffer<CharT>();
		Buffer.Resize(Lhs.Size + Rhs.Size);
		XE::Memcpy(Buffer.Data, Lhs.Data, sizeof(CharT) * Lhs.Size);
		XE::Memcpy(Buffer.Data + Lhs.Size, Rhs.Data, sizeof(CharT) * Rhs.Size);
		return Buffer;
	}

	template TView<char> operator +(TView<char> Lhs, TView<char> Rhs);
	template TView<wchar_t> operator +(TView<wchar_t> Lhs, TView<wchar_t> Rhs);
	template TView<char8_t> operator +(TView<char8_t> Lhs, TView<char8_t> Rhs);
	template TView<char16_t> operator +(TView<char16_t> Lhs, TView<char16_t> Rhs);
}
