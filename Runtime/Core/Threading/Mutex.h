#pragma once

#include "ThreadingInc.h"

namespace XE
{
	class CORE_API FMutex
	{
	public:
		FMutex();
		~FMutex();
		FMutex(const FMutex &) = delete;
		FMutex(FMutex && That) noexcept :Handle(That.Handle)
		{
			That.Handle = nullptr;
		}

		void Lock() const;
		void Unlock() const;
		ptr_t GetHandle() const { return Handle; }

	private:
		ptr_t Handle = nullptr;
#ifdef XE_DEBUG
		mutable uint32_t ThreadIdLocked = 0;
#endif
	};
}