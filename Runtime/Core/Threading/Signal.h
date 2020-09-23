#pragma once

#include "ThreadingInc.h"

namespace XE
{
	class CORE_API FSignal
	{
	public:
		FSignal();
		~FSignal();
		FSignal(const FSignal &) = delete;
		FSignal(FSignal && That) noexcept :Handle(That.Handle)
		{
			That.Handle = nullptr;
		}

		void Signal() const;
		void Wait() const;
		ptr_t GetHandle() const { return Handle; }
		
	private:
		ptr_t Handle = nullptr;
#ifdef XE_DEBUG
		mutable uint32_t ThreadIdLocked = 0;
#endif
	};
}