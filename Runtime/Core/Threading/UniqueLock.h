#pragma once

#include "ThreadingInc.h"

namespace XE
{
	template<typename MutexT>
	class TUniqueLock
	{
	public:
		TUniqueLock(MutexT & MutexIn) : Mutex(MutexIn)
		{
			Mutex.Lock();
		}
		TUniqueLock(const TUniqueLock &) = delete;
		TUniqueLock(TUniqueLock && That) = delete;
		~TUniqueLock()
		{
			Mutex.Unlock();
		}

		MutexT & GetMutex() { return Mutex; }
		const MutexT & GetMutex() const { return Mutex; }
	private:
		MutexT & Mutex;
	};
}