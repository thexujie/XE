#include "PCH.h"
#include "Thread.h"
#include "Platform/Win32/Win32.h"

namespace XE
{
	FThread::FThread(TFunction<void()> FunctionIn)
	{
		struct SThreadFunctionArgs
		{
			TFunction<void()> Function;
		};
		
		auto FuncEntry = [](void * Args)->uint32_t
		{
			SThreadFunctionArgs * FunctionArgs = (SThreadFunctionArgs *)Args;
			FunctionArgs->Function();
			Delete(FunctionArgs);
			//_endthreadex(0);
			return 0;
		};

		SThreadFunctionArgs * FunctionArgs = New<SThreadFunctionArgs>();
		FunctionArgs->Function = FunctionIn;
		Handle = (ptr_t)_beginthreadex(nullptr, 0, FuncEntry, FunctionArgs, 0, &Id);
	}
	
	void FThread::Join()
	{
		if (!Id)
			return;

		[[maybe_unused]] DWORD Wait = ::WaitForSingleObject(Handle, INFINITE);
		Id = 0;
	}

	FThread & FThread::operator =(FThread && That) noexcept
	{
		Id = That.Id;
		Handle = That.Handle;
		That.Id = 0;
		That.Handle = nullptr;
		return *this;
	}
}