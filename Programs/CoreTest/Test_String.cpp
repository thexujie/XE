#include "PCH.h"

#include <iostream>
#include <regex>

#ifdef XE_TEST


namespace XE
{
	template<typename T>
	void CheckLibFormatAble2()
	{
		static_assert(IsLibFormatAbleV<T>, __FUNCTION__ " Custom type need a TFormatter<T> or ToString() member.");
	};

	template<typename T = void, typename ...ArgsT>
	void CheckLibFormat2();

	template<typename T, typename ...ArgsT>
	void CheckLibFormat2()
	{
		CheckLibFormatAble2<RemoveCVRefT<T>>();
		CheckLibFormat2<ArgsT...>();
	}

	template<>
	inline void CheckLibFormat2<>()
	{
	}
	
	template<typename CharT, typename ...ArgsT>
	TView<CharT> FormatImpl2(TView<CharT> Formal, ArgsT && ...Args)
	{
		//CheckLibFormatAble2<ArgsT...>();
		CheckLibFormat2<ArgsT...>();

		if constexpr (ConjunctionV<IsLibFormatAble<RemoveCVRefT<ArgsT>>...>)
		{
			fmt::basic_memory_buffer<CharT> & FormatBuffer = GetThreadLocalFormatBuffer<CharT>();
			fmt::format_to(FormatBuffer, fmt::basic_string_view<CharT>(Formal.Data, Formal.Size), Forward<ArgsT>(Args)...);
			return TView<CharT>(FormatBuffer.begin(), FormatBuffer.size());
		}
		else
			return Formal;
	}
	
	class FTTT
	{
		
	};
	
	class Test_String
	{
	public:
		Test_String()
		{
			FString TestText(Str("xyzabcdcccddd  "));
			TestText.Trim(Str("cdxy 5zab"));
			
			TestText.TrimEnd();
			TestText.TrimEnd(Str("cd"));
			TestText.TrimStart(Str("xy"));

			TestText.Trim(Str("zab"));

			FDateTime DateTime = FDateTime::Now();
			FStringView DateTimeStr = DateTime.Format(Str("yyyy-MM-dd HH-mm-ss-fff"));

			auto DateTimeStrE = DateTimeStr == TestText;
			Assert(FDateTime::Now() == DateTime);
			FDateTime DateTime2(DateTime.Year, DateTime.Month, DateTime.Day, DateTime.Hour, DateTime.Minute, DateTime.Second,
				DateTime.Millisecond, DateTime.Microsecond, DateTime.Nanosecond);
			return;
			
			TFormatter<char> Converter;
			auto ss = Converter.Parse(Str("A"));
			auto ss2 = Converter.Format('X');

			FStringView SVA;
			FStringView SVB;
			SVA <=> SVB;
			FString String1 = FString(Str("Test_String")).Replace(Str("_S"), Str("1"));
			FString String2 = FString(Str("ABC")).Replace(Str("AB"), Str("123456789"));
			FString String = Str("ABC ABC ABCD AB C ABCD");
			String.Replace(Str("ABC"), Str("123456"));
			String.Clear();
			std::cout << "Test_String OK" << std::endl;
		}
	}Test_String_Instance;
}

#endif
