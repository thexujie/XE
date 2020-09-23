#include "PCH.h"
#include "Debug.h"

namespace XE
{
	namespace Debug
	{
		void WriteLine(FStringView String)
		{
			::OutputDebugStringW(Strings::ToWide(String).Data);
			::OutputDebugStringW(L"\r\n");
		}

		void WriteLine(FWStringView String)
		{
			::OutputDebugStringW(String.Data);
			::OutputDebugStringW(L"\r\n");
		}
	}
}
