#include "PCH.h"
#include "Signal.h"
#include "../Platform/Win32/Win32.h"

namespace XE
{
	FSignal::FSignal()
	{
		Handle = CreateEventExW(NULL, NULL, 0, SYNCHRONIZE | EVENT_MODIFY_STATE);

	}
	FSignal::~FSignal()
	{
		if (Handle)
		{
			CloseHandle(Handle);
			Handle = NULL;
		}
	}

	void FSignal::Signal() const
	{
		SetEvent(Handle);
	}

	void FSignal::Wait() const
	{
		WaitForSingleObject(Handle, INFINITE);
	}
}