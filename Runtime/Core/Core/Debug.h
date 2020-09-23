#pragma once

#include "CoreInc.h"

namespace XE
{
	namespace Debug
	{
		CORE_API void WriteLine(FStringView String);
		CORE_API void WriteLine(FWStringView String);

		template<typename ...ArgsT>
		void WriteLineF(FStringView Formal, ArgsT &&...Args)
		{
#ifdef XE_DEBUG
			WriteLine(XE::Format(Formal, Forward<ArgsT>(Args)...));
#endif
		}

		template<typename ...ArgsT>
		void WriteLineF(FWStringView Formal, ArgsT &&...Args)
		{
#ifdef XE_DEBUG
			WriteLine(XE::Format(Formal, Forward<ArgsT>(Args)...));
#endif
		}
	}
}

