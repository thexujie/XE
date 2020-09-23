#include "PCH.h"
#include "D2DBitmap.h"
#include "D2DDevice.h"
#include "D2DGraphics.h"
#include "Direct2D.h"
#include "Core/Platform/Win32/Win32.h"

using namespace XE;
using namespace XE::Direct2D;

FCounter<float32_t, 3> FpsCounter(1000);
TSharedPtr<FD2DDevice> D2DDevice;
TSharedPtr<FPixmap> GBitmap;
TSharedPtr<FGraphics> GrpGraphics;

#define DIB_WIDTHBYTES(bits) ((((bits) + 31)>>5)<<2)

enum class EDrawMode
{
	Bitmap,
	Window,
};

EDrawMode DrawMode = EDrawMode::Bitmap;

void OnDraw(FGraphicsService & GrpService, FGraphics & D2DGraphics)
{
	FpsCounter.Accumulate(1);
	D2DGraphics.Clear(Colors::AliceBlue);

	//D2DGraphics.DrawRectangle({ 50, 50, 700, 700 }, { Colors::BurlyWood, 3.0f });
	//D2DGraphics.DrawLine({ 100.f, 400.f }, { 700, 400 }, { 0xff00ff00, 2.0f });
	//D2DGraphics.DrawLine({ 400.f, 100.f }, { 400, 700 }, { 0xff00ff00, 2.0f });
	//D2DGraphics.DrawRectangle({ 300, 300.f, 200, 200 }, { Colors::Brown, Colors::Blue, 1.0f });
	//D2DGraphics.DrawRectangle({ 300, 300, 150, 150 }, { Colors::Red });
	//D2DGraphics.DrawString(Str("1234个无法解析的外部命令"), { 400, 400 }, { Font, ETextFlag::AlignCenter }, FPathFormat::Stroke(Colors::Red));
	//D2DGraphics.DrawString(Str("1234个无法解析的外部命令"), { 300, 300.f, 200, 200 }, { { FString::Empty, 20 }, ETextFlag::AlignCenter }, Colors::Red);
	//D2DGraphics.DrawString(Str("1234个无法解析的外部命令"), { 300, 300.f, 200, 200 }, { { FString::Empty, 20 }, ETextFlag::AlignCenter | ETextFlag::LineAlignFar }, Colors::Red);
	//D2DGraphics.DrawString(Str("1234个无法解析的外部命令"), { 300, 300.f, 200, 200 }, { { FString::Empty, 20 }, ETextFlag::AlignCenter | ETextFlag::LineAlignCenter }, Colors::Red);

	//D2DGraphics.DrawPath(
	//	{
	//		{ EPathCommand::MoveTo, { 150, 150 } },
	//		{ EPathCommand::LineTo, { 250, 150 } },
	//		{ EPathCommand::LineTo, { 250, 250 } },
	//		{ EPathCommand::Point, { 250, 250 } },
	//		{ EPathCommand::Point, { 300, 250 } },
	//		{ EPathCommand::Bezier4To, { 400, 400 } },
	//	},
	//	{ Colors::Black, 2.0f });

	//for (int Row = 0; Row < 60; ++Row)
	//	D2DGraphics.DrawString(Str(R"TEX(“Direct2D.exe”(Win32): 已加载“C:\Windows\System32\windows.storage.dll”。包含/排除设置已禁用符号加载。“Direct2D.exe”(Win32): 已加载“C:\Windows\System32\windows.storage.dll”。包含/排除设置已禁用符号加载。)TEX"),
	//		{ 30, 30 + Row * 25.0f }, { { FString::Empty, 20 } }, Colors::Black);
	TVector<FPathPoint> PathPoints = GrpService.StringToPath(Str("PD加载"), { Float32Max, Float32Max }, { { FString::Empty, FontWeights::Bold }, 400, ETextFlags::AlignLeading });

	float CircleRadius = 3.0f;
	float PointStrokeWidth = 1.0f;

	FColor32 PointFill = Colors::DarkGreen;
	FColor32 PointStroke = Colors::Gray;

	FColor32 CPointFill = Colors::DarkRed;

	FColor32 LineStroke = Colors::Black;
	float LineStrokeWidth = 1.0f;

	D2DGraphics.FillPath(PathPoints, {0, 0}, FSolidBrush{ Colors::Gray });
	D2DGraphics.DrawPath(PathPoints, { 0, 0 }, FSolidBrush{ Colors::Red });

	D2DGraphics.DrawString(Format(Str("FPS={}"), (int)FpsCounter.Average()), { 0, 0 }, { { FString::Empty, FontWeights::Bold }, 20, ETextFlags::AlignLeading }, FSolidBrush(Colors::Red));
	D2DGraphics.Flush();
}

void Draw(HWND hWnd)
{
	if (!D2DDevice)
	{
		D2DDevice = MakeShared<FD2DDevice>();
		D2DDevice->Initialize();
	}

	if (DrawMode == EDrawMode::Bitmap)
	{
		SizeU WindowSize = FWindowUtils::GetWindowSize(hWnd);
		if (!GBitmap)
		{
			GBitmap = D2DDevice->CreateBitmap(WindowSize, EFormat::BGRA);
			GrpGraphics = D2DDevice->CreateGraphics(*GBitmap);
		}

		if (GBitmap->GetSize() != WindowSize)
		{
			GBitmap = D2DDevice->CreateBitmap(FWindowUtils::GetWindowSize(hWnd), EFormat::BGRA);
			GrpGraphics = D2DDevice->CreateGraphics(*GBitmap);
		}

		OnDraw(*D2DDevice, *GrpGraphics);
		IWICBitmap * WICBitmap = ((FD2DBitmap *)GBitmap.Get())->GetWICBitmap();
		try
		{
			BITMAPINFO bminfo;
			ZeroMemory(&bminfo, sizeof(bminfo));
			bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bminfo.bmiHeader.biWidth = GBitmap->GetSize().Width;
			bminfo.bmiHeader.biHeight = -(LONG)GBitmap->GetSize().Height;
			bminfo.bmiHeader.biPlanes = 1;
			bminfo.bmiHeader.biBitCount = 32;
			bminfo.bmiHeader.biCompression = BI_RGB;

			TReferPtr<IWICBitmapLock> WICBitmapLock;
			WICRect Rect = { 0, 0, (int)GBitmap->GetSize().Width, (int)GBitmap->GetSize().Height };
			HRESULT Result = WICBitmap->Lock(&Rect, WICBitmapLockRead, WICBitmapLock.GetPP());

			uint32_t DataSize = 0;
			WICInProcPointer InProcPointer = nullptr;
			WICBitmapLock->GetDataPointer(&DataSize, &InProcPointer);

			HDC hdc = GetDC(hWnd);
			::SetDIBitsToDevice(hdc, 0, 0, GBitmap->GetSize().Width, GBitmap->GetSize().Height, 0, 0, 0, GBitmap->GetSize().Height, InProcPointer, &bminfo, DIB_RGB_COLORS);
			ReleaseDC(hWnd, hdc);
		}
		catch (...)
		{
			//if (hDIBBitmap)
			//	DeleteObject(hDIBBitmap);
			//// Rethrow the exception, so the client code can handle it
			//throw;
		}
	}
	else if (DrawMode == EDrawMode::Window)
	{
		if (!GrpGraphics)
			GrpGraphics = D2DDevice->CreateGraphics(hWnd);
		OnDraw(*D2DDevice, *GrpGraphics);
	}
}

static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_PAINT)
	{
		Draw(hWnd);
		PAINTSTRUCT pt;
		HDC hdc = BeginPaint(hWnd, &pt);
		EndPaint(hWnd, &pt);
	}
	else if (message == WM_KEYDOWN) { }
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
	else if (message == WM_MOUSEMOVE) { }
	else if (message == WM_MOUSEWHEEL) { }
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

struct Point
{
	int x;
	int y;
	Point(int a, int b) : x(a), y(b) {}
	auto operator<=>(const Point &) const = default; // 预置 operator<=>
};


int _tmain(int argc, const char * args[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(673);
	FGlobalLogger GlobalLogger(Str("../Data/Local/Direct2D.log"), ELogLevel::LogLevel_Dbg);
	LogInfo(Str("Main function!!!!"));
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

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
	wcex.lpszClassName = Win32::WINDOW_CLASS_NAME;
	wcex.hIconSm = NULL;

	RegisterClassExW(&wcex);

	
	HWND hWnd = CreateWindowExW(
		0, Win32::WINDOW_CLASS_NAME, L"Title", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080,
		NULL, NULL, hInstance, NULL);

	::ImmAssociateContext(hWnd, NULL);
	::ShowWindow(hWnd, SW_NORMAL);
	::UpdateWindow(hWnd);

	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		DWORD dwWait = MsgWaitForMultipleObjectsEx(0, NULL, 0, QS_ALLEVENTS, MWMO_ALERTABLE);
		//switch (dwWait)
		//{
		//default:
		//	break;
		//}

		Draw(hWnd);

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
