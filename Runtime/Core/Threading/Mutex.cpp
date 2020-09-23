#include "PCH.h"
#include "Mutex.h"
#include "../Platform/Win32/Win32.h"

namespace XE
{
	FMutex::FMutex()
	{
		Handle = new CRITICAL_SECTION();
		InitializeCriticalSectionAndSpinCount((CRITICAL_SECTION *)Handle, 200);

	}
	FMutex::~FMutex()
	{
		if (Handle)
			delete (CRITICAL_SECTION *)Handle;
	}

	void FMutex::Lock() const
	{
		EnterCriticalSection((CRITICAL_SECTION *)Handle);

#ifdef XE_DEBUG
		Atomics::FetchExchange(ThreadIdLocked, FSystem::ThreadId());
#endif
	}

	void FMutex::Unlock() const
	{
		LeaveCriticalSection((CRITICAL_SECTION *)Handle);
#ifdef XE_DEBUG
		Atomics::FetchExchange(ThreadIdLocked, 0u);
#endif
	}
}