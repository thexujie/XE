#include "PCH.h"
#include "FileLinePeriod.h"
#include "DateTime.h"

namespace XE
{
	bool FFileLinePeriod::Check() const
	{
        static thread_local std::map<TTuple<const char *, int>, uint64_t> __LastCheckMilliseconds;
		uint64_t Milliseconds = FDateTime::SteadyMilliseconds();

        uint64_t LastCheckMilliseconds = __LastCheckMilliseconds[Id];
        if (Milliseconds < LastCheckMilliseconds)
            return false;

		if ((Milliseconds - LastCheckMilliseconds) * Count < static_cast<uint64_t>(Period))
			return false;

        __LastCheckMilliseconds[Id] = Milliseconds;
        return true;
	}
}
