#include "PCH.h"
#include "Delegate.h"

namespace XE
{
	uintx_t FEvent::CreateEventId()
	{
		static uintx_t GEventId = 0;
		return Atomics::IncFetch(GEventId);
	}
}