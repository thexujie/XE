#include "PCH.h"
#include "D2DRenderTarget.h"

namespace XE::Direct2D
{
	EError FD2DRenderTarget::InitializeForHWND(HWND hWnd)
	{
		SizeU WindowSize = FWindowUtils::GetWindowSize(hWnd);
		HRESULT Result = D2DDevice.GetD2D1Factory()->CreateHwndRenderTarget(
				D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(hWnd, {WindowSize.Width, WindowSize.Height}),
				D2D1HwndRenderTarget.GetPP());
		if (Win32::Failed(Result))
		{
			LogError(Str("D2D1CreateFactory Failed: {}"), Result);
			return EError::Inner;
		}
		return EError::OK;
	}
}
