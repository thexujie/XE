#pragma once

#include "StringView.h"

namespace XE
{
	struct CORE_API FSystem
	{
		FSystem() = delete;
		
		static ptr_t Instance();
		static uint32_t ThreadId();
		static uint32_t ProcessId();
		static void SetThreadDescription(FStringView Description);
		static void SetThreadDescription(uint32_t ThreadId, FStringView Description);
		static void DebugOutput(FStringView String);
		static void DebugOutput(FWStringView String);
	};
}
