#include "PCH.h"
#include "Inc.h"
#include "SampleWorld.h"

static const TCHAR * WINDOW_CLASS_NAME = TEXT("WINDOW_CLASS_NAME");

extern FSampleWorld * SampleWorld;
static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_PAINT)
	{
		PAINTSTRUCT pt;
		HDC hdc = BeginPaint(hWnd, &pt);
		EndPaint(hWnd, &pt);
	}
	else if (message == WM_KEYDOWN)
	{
		if (SampleWorld)
			SampleWorld->OnKeyDown(uint32_t(wParam), uint32_t(lParam));
	}
	else if (message == WM_LBUTTONDOWN)
	{
		::SetCapture(hWnd);
		if (SampleWorld)
			SampleWorld->OnMouseDownL(PointI(LowAs<int16_t>(Low(lParam)), HighAs<int16_t>(Low(lParam))));
	}
	else if (message == WM_LBUTTONUP)
	{
		::ReleaseCapture();
		if (SampleWorld)
			SampleWorld->OnMouseUpL(PointI(LowAs<int16_t>(Low(lParam)), HighAs<int16_t>(Low(lParam))));
	}
	else if (message == WM_MOUSEMOVE)
	{
		if (SampleWorld)
			SampleWorld->OnMouseMove(PointI(LowAs<int16_t>(Low(lParam)), HighAs<int16_t>(Low(lParam))));
	}
	else if (message == WM_MOUSEWHEEL)
	{
		if (SampleWorld)
			SampleWorld->OnMouseWheel(PointI{ 0, HighAs<int16_t>(Low(wParam)) / WHEEL_DELTA });
	}
	else if (message == WM_CLOSE)
	{
		if (SampleWorld)
			SampleWorld->Finalize();
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

int main_hair();

#include "Direct2D/Direct2D.h"
#include "Direct2D/D2DModule.h"
#include <Direct2D\D2DDevice.h>

int _tmain(int argc, const char *args[])
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	//return main_hair();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(673);
	FGlobalLogger GlobalLogger(Str("../Data/Local/Sample.log"), ELogLevel::LogLevel_Dbg);
	LogInfo(Str("Main function!!!!"));

	FApplication Application;

	Application.RegisterService(TypeOf<FGraphicsService>(), TypeOf<Direct2D::FD2DDevice>());

	//FRHIFactory * CreateRHIFactory()
	//{
	//	FRHIFactory * RHIFactory = new RHID3D12::FRHID3D12Factory();
	//	RHIFactory->Load();
	//	return RHIFactory;
	//}

	
	//{
	//	// Test Shader Compile
	//	FShader TestShader(EShaderType::Vertex, Str("../Content/Shaders/TestVertexShader.hlsl"), Str("VSMain"));
	//}

	//TMeshAssemblerRegister<FLocalMeshAssembler> MeshAssemblerRegister_BlendMeshAssembler2;

	HINSTANCE hInstance = (HINSTANCE)FSystem::Instance();

	WNDCLASSEXW wcex = { sizeof(WNDCLASSEXW) };

	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.style = 0;
	wcex.lpfnWndProc = MainWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = /*::LoadCursor(NULL, IDC_ARROW)*/NULL;
	//wcex.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = WINDOW_CLASS_NAME;
	wcex.hIconSm = NULL;

	RegisterClassExW(&wcex);

	HWND hWnd = CreateWindowExW(
		0, WINDOW_CLASS_NAME, L"Title", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080,
		NULL, NULL, hInstance, NULL);

	::ImmAssociateContext(hWnd, NULL);
	::ShowWindow(hWnd, SW_NORMAL);
	::UpdateWindow(hWnd);

	FSampleWorld SampleWorld;
	SampleWorld.Initialize(hWnd);
	
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		SampleWorld.Game_Update();
		
		DWORD dwWait = MsgWaitForMultipleObjectsEx(0, NULL, 0, QS_ALLEVENTS, MWMO_ALERTABLE);
		//switch (dwWait)
		//{
		//default:
		//	break;
		//}

		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	return (int32_t)msg.wParam;
}
