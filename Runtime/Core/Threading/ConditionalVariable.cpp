#include "PCH.h"
#include "ConditionalVariable.h"
#include "../Platform/Win32/Win32.h"

namespace XE
{
	FConditionalVariable::FConditionalVariable()
	{
		InitializeConditionVariable((CONDITION_VARIABLE *)&Handle);
	}
	
	FConditionalVariable::~FConditionalVariable()
	{
		
	}

	void FConditionalVariable::NotifyOne()
	{
		WakeConditionVariable((CONDITION_VARIABLE *)&Handle);
	}
	
	void FConditionalVariable::NotifyAll()
	{
		WakeAllConditionVariable((CONDITION_VARIABLE *)&Handle);
	}

	void FConditionalVariable::Wait(FMutex & Mutex)
	{
		SleepConditionVariableCS((CONDITION_VARIABLE *)&Handle, (CRITICAL_SECTION *)Mutex.GetHandle(), INFINITE);
	}

	void FConditionalVariable::Wait(FMutex & Mutex, TFunction<bool()> Condition)
	{
		while(!Condition())
		{
			Mutex.Unlock();
			SleepConditionVariableCS((CONDITION_VARIABLE *)&Handle, (CRITICAL_SECTION *)Mutex.GetHandle(), INFINITE);
		}
	}
}