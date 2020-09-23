#include "PCH.h"

#pragma comment(lib, "imm32.lib")

namespace XE
{
	void Log(ELogLevel LogLevel, FStringView String)
	{
		Logger().WriteLine(LogLevel, Str("Sample"), String);
	}
}
