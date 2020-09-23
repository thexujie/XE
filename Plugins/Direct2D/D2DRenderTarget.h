#pragma once

#include "D2DInc.h"
#include "D2DDevice.h"

namespace XE::Direct2D
{
	class FD2DRenderTarget
	{
	public:
		FD2DRenderTarget(FD2DDevice & FD2DDeviceIn) : D2DDevice(FD2DDeviceIn) {}
		~FD2DRenderTarget() = default;

		EError InitializeForHWND(HWND hWnd);
		
	private:
		FD2DDevice & D2DDevice;
		TReferPtr<ID2D1HwndRenderTarget> D2D1HwndRenderTarget;
	};
}
