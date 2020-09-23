#include "PCH.h"
#include "System.h"
#include "../Platform/Win32/Win32.h"

namespace XE
{
	ptr_t FSystem::Instance()
	{
		return (ptr_t)GetModuleHandleW(NULL);
	}

	uint32_t FSystem::ThreadId()
	{
		return (uint32_t)GetCurrentThreadId();
	}

	uint32_t FSystem::ProcessId()
	{
		return (uint32_t)GetProcessId(GetCurrentProcess());
	}

	void FSystem::SetThreadDescription(FStringView Description)
	{
		::SetThreadDescription(GetCurrentThread(), Strings::ToWide(Description).GetData());
	}
	
	void FSystem::SetThreadDescription(uint32_t ThreadId, FStringView Description)
	{
		HANDLE hThread = ::OpenThread(THREAD_SET_LIMITED_INFORMATION, FALSE, ThreadId);
		if (hThread)
		{
			::SetThreadDescription(hThread, Strings::ToWide(Description).GetData());
			CloseHandle(hThread);
		}
	}
	
	void FSystem::DebugOutput(FStringView String)
	{
		::OutputDebugStringW(Strings::ToWide(String).Data);
		::OutputDebugStringW(L"\r\n");
	}
	
	void FSystem::DebugOutput(FWStringView String)
	{
		::OutputDebugStringW(String.Data);
		::OutputDebugStringW(L"\r\n");
	}
}
