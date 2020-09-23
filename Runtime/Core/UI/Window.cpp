#include "PCH.h"
#include "Window.h"
#include "../Platform/Win32/Win32.h"

namespace XE
{
	SizeU FWindowUtils::GetWindowSize(ptr_t WindowHandle)
	{
		RECT ClientRect = {};
		GetClientRect(HWND(WindowHandle), &ClientRect);
		SizeU WindowSize = { uint32_t(ClientRect.right - ClientRect.left), uint32_t(ClientRect.bottom - ClientRect.top) };
		return WindowSize;
	}
}