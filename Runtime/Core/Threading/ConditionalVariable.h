#pragma once

#include "ThreadingInc.h"
#include "Mutex.h"

namespace XE
{
	class CORE_API FConditionalVariable
	{
	public:
		FConditionalVariable();
		FConditionalVariable(const FConditionalVariable &) = delete;
		FConditionalVariable(FConditionalVariable && That) noexcept : Handle(That.Handle) { That.Handle = nullptr; }
		~FConditionalVariable();

		void NotifyOne();
		void NotifyAll();
		void Wait(FMutex & Mutex);
		void Wait(FMutex & Mutex, TFunction<bool()> Condition);

		template<typename LockT>
		void Wait(LockT & Lock, TFunction<bool()> Condition)
		{
			Wait(Lock.GetMutex(), Condition);
		}

		template<typename LockT>
		void Wait(LockT & Lock)
		{
			Wait(Lock.GetMutex());
		}
	private:
		ptr_t Handle = nullptr;
#ifdef XE_DEBUG
		mutable uint32_t ThreadIdLocked = 0;
#endif
	};
}
