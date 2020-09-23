#pragma once

#include "Types.h"
#include "Formatter.h"
#include "StringView.h"

namespace XE
{
	enum ELogLevel
	{
		LogLevel_Dbg = 0,
		LogLevel_Inf,
		LogLevel_War,
		LogLevel_Err,
	};

	class CORE_API FLoggerStream
	{
	public:
		virtual ~FLoggerStream() = default;
		virtual EError Write(ELogLevel Level, FStringView Category, FStringView String) = 0;
		virtual EError WriteLine(ELogLevel Level, FStringView Category, FStringView String) = 0;
	};

	CORE_API FLoggerStream & Logger();

	void Log(ELogLevel LogLevel, FStringView String);

	template<typename ...ArgsT>
	void LogDbg(FStringView Formal, ArgsT &&...Args)
	{
#ifdef _DEBUG
		Log(LogLevel_Dbg, Format(Formal, Forward<ArgsT>(Args)...));
#endif
	}

	template<typename ...ArgsT>
	void LogInfo(FStringView Formal, ArgsT &&...Args)
	{
		Log(LogLevel_Inf, Format(Formal, Forward<ArgsT>(Args)...));
	}

	template<typename ...ArgsT>
	void LogWarning(FStringView Formal, ArgsT &&...Args)
	{
		Log(LogLevel_War, Format(Formal, Forward<ArgsT>(Args)...));
	}

	template<typename ...ArgsT>
	void LogError(FStringView Formal, ArgsT &&...Args)
	{
		Log(LogLevel_Err, Format(Formal, Forward<ArgsT>(Args)...));
	}
}
