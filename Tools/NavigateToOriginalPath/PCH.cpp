#include "PCH.h"

#pragma comment(lib, "shlwapi.lib")

namespace XE
{
	void Log(ELogLevel LogLevel, FStringView String)
	{
		Logger().WriteLine(LogLevel, Str("NavigateToOriginalPath"), String);
	}
}
