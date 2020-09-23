#include "PCH.h"

#include "SampleForm.h"

using namespace XE;

static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_PAINT)
	{
		PAINTSTRUCT pt;
		HDC hdc = BeginPaint(hWnd, &pt);
		EndPaint(hWnd, &pt);
	}
	else if (message == WM_KEYDOWN) {}
	else if (message == WM_SIZE)
	{
	}
	else if (message == WM_LBUTTONDOWN)
	{
		::SetCapture(hWnd);
	}
	else if (message == WM_LBUTTONUP)
	{
		::ReleaseCapture();
	}
	else if (message == WM_MOUSEMOVE) {}
	else if (message == WM_MOUSEWHEEL) {}
	else if (message == WM_CLOSE)
	{
		PostQuitMessage(0);
	}
	//else if (message == WM_CLOSE)
	//{
	//	if (Render)
	//		Render->Stop();
	//	PostQuitMessage(0);
	//}
	return DefWindowProc(hWnd, message, wParam, lParam);
}


int SampleMain()
{
	FApplication Application;
	FGlobalLogger GlobalLogger(Str("../Data/Local/UISample.log"), ELogLevel::LogLevel_Dbg);
	LogInfo(Str("Enter Main"));

	Application.RegisterType<FControlTemplate, FObject>();

	Application.RegisterType<FUIObject, FObject>();
	Application.RegisterType<UVisual, FUIObject>();
	Application.RegisterType<UElement, UVisual>();

	Application.RegisterType<UContentPresenter, UElement>();
	Application.RegisterType<UTextBlock, UElement>();
	Application.RegisterType<UDecorator, UElement>();

	Application.RegisterType<UControl, UElement>();
	Application.RegisterType<UContentControl, UControl>();

	Application.RegisterType<UTitleBar, UControl>();
	Application.RegisterType<UPanel, UControl>();

	Application.RegisterType<UStackPanel, UPanel>();
	Application.RegisterType<UVBox, UStackPanel>();
	Application.RegisterType<UHBox, UStackPanel>();

	Application.RegisterType<UButton, UContentControl>();
	Application.RegisterType<UForm, UContentControl>();
	Application.RegisterType<USampleForm, UForm>();

	Application.RegisterType<UBorder, UDecorator>();

	Application.RegisterType<FBrush, FObject>();
	Application.RegisterType<FSolidBrush, FBrush>();
	Application.RegisterType<FStyle>();

	Application.RegisterType<FGraphicsService, IService>();
	Application.RegisterType<Direct2D::FD2DDevice, FGraphicsService>();

	Application.RegisterService<FGraphicsService, Direct2D::FD2DDevice>();

	for (FStringView FilePath : FDirectory::GetFiles(Str("../Content/UI/ControlTemplates/")))
	{
		FResourceDictionary ResourceDictionary(FilePath);
		Application.ResourceDirectory.MergeResources(FilePath);
	}

	//USampleForm Form({ 0, 0, 1920, 1080 });
	//Form.SetFormFlag(EFormFlag::QuitOnClose, true);
	//Form.SetTitle(Str("Main Form"));
	//Form.Show(UI::EShowMode::Default, UI::EShowPosition::CenterScreen);

	UI::FUIFactory UILoader;
	TReferPtr<UForm> Form = StaticPointerCast<UForm>(UILoader.LoadFromXml(Str("SampleForm.xeml")));
	Form->SetFormFlag(EFormFlags::QuitOnClose, true);
	//Form->SetTitle(Str("Main Form"));
	Form->Show(UI::EShowMode::Default, UI::EShowPosition::CenterScreen);
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		DWORD dwWait = MsgWaitForMultipleObjectsEx(0, NULL, INFINITE, QS_ALLEVENTS, MWMO_ALERTABLE);
		switch (dwWait)
		{
		default:
			break;
		}

		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}


	//CoUninitialize();
	return (int32_t)msg.wParam;
}

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd
)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(1095);
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	//CoInitialize(NULL);
	int Result = SampleMain();
	CoUninitialize();
	return Result;
}
