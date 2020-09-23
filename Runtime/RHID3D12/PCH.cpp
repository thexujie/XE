#include "PCH.h"

namespace XE
{
	void Log(ELogLevel LogLevel, FStringView String)
	{
		Logger().WriteLine(LogLevel, Str("RHI::D3D12"), String);
	}
}
