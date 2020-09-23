#include "PCH.h"
#include "Windows.h"

namespace XE::Win32
{
	HINSTANCE Instance()
	{
		return GetModuleHandleW(NULL);
	}
}
