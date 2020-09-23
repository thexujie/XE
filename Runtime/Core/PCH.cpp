#include "PCH.h"

namespace XE
{
	void Log(ELogLevel LogLevel, FStringView String)
	{
		Logger().WriteLine(LogLevel, Str("Core"), String);
	}
}
