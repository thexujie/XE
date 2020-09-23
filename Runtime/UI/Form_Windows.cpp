#include "PCH.h"
#include "Form.h"
#include "Core/Platform/Win32/Win32.h"

#ifdef XE_WINDOWS
namespace XE::UI
{
    static POINT LParamPoint(intx_t lParam)
    {
#if XE_X64
        POINT Point = { LowAs<int16_t>(Low(lParam)), HighAs<int16_t>(Low(lParam)) };
#else
        POINT Point = { LowAs<int16_t>(lParam), HighAs<int16_t>(lParam) };
#endif
        return Point;
    }

    static Vec2I LParamVec2I(intx_t lParam)
    {
#if XE_X64
        Vec2I Point = { LowAs<int16_t>(Low(lParam)), HighAs<int16_t>(Low(lParam)) };
#else
        Vec2I Point = { LowAs<int16_t>(lParam), HighAs<int16_t>(lParam) };
#endif
        return Point;
    }

    static Vec2U LParamVec2U(intx_t lParam)
    {
#if XE_X64
        Vec2U Point = { LowAs<uint16_t>(Low(lParam)), HighAs<uint16_t>(Low(lParam)) };
#else
        Vec2U Point = { LowAs<uint16_t>(lParam), HighAs<uint16_t>(lParam) };
#endif
        return Point;
    }
	
	static TTuple<uint32_t, uint32_t> MakeWindowStyle(EFormStyle FormStyle, EFormFlags FormFlags)
	{
        uint32_t Style = 0;
        uint32_t StyleEx = 0;
		switch(FormStyle)
		{
        case EFormStyle::Default:
            Style = WS_OVERLAPPEDWINDOW;
			break;
		case EFormStyle::System:
            Style = WS_OVERLAPPEDWINDOW;
            //Style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
            //SetFlag<uint32_t>(Style, WS_MINIMIZEBOX, !FormFlags.Any(EFormFlag::NoMinimizeBox));
            //SetFlag<uint32_t>(Style, WS_MAXIMIZEBOX, !FormFlags.Any(EFormFlag::NoMaximizeBox));
            //SetFlag<uint32_t>(Style, WS_THICKFRAME, !FormFlags.Any(EFormFlag::NoResizeBorder));
            break;
		case EFormStyle::Layered:
            SetFlag<uint32_t>(StyleEx, WS_EX_LAYERED, true);
			break;
		default:
            break;
		}

        SetFlag<uint32_t>(StyleEx, WS_EX_TOOLWINDOW, FormFlags * EFormFlags::NoTaskButton);
        SetFlag<uint32_t>(StyleEx, WS_EX_TOPMOST, FormFlags * EFormFlags::TopMose);
		
        //Style = WS_OVERLAPPEDWINDOW;
        //StyleEx = 0;

        return { Style, StyleEx };
	}

    static thread_local UForm * __CurrentForm = nullptr;
    LRESULT CALLBACK DefaultWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
    {
    	if (__CurrentForm)
    	{
            if (GetPropW(hWnd, Win32::WINDOW_PROP_THIS_PTR))
                throw EError::BadData;

			SetPropW(hWnd, Win32::WINDOW_PROP_THIS_PTR, (HANDLE)__CurrentForm);
            SetPropW(hWnd, Win32::WINDOW_PROP_DLG_RESULT, (HANDLE)0);
            __CurrentForm->WindowHandle = hWnd;
            __CurrentForm = nullptr;
    	}

        UForm * Form = (UForm *)(void *)GetPropW((HWND)hWnd, Win32::WINDOW_PROP_THIS_PTR);
        Assert(Form, L"Not XE UForm");

        switch (Message)
        {
        case WM_MOVE:
        case WM_MOUSEMOVE:
        case WM_NCMOUSEMOVE:
        case WM_SIZE:
        case WM_NCHITTEST:
        case WM_PAINT:
        case WM_SETCURSOR:
        case WM_WINDOWPOSCHANGED:
        case WM_WINDOWPOSCHANGING:
        case WM_GETMINMAXINFO:
        case WM_MOVING:
        case WM_IME_NOTIFY:
        case WM_ERASEBKGND:
        case WM_NCPAINT:
        case WM_NCMOUSELEAVE:
            break;
        default:
            break;
        }

        return Form->HandleMessage(Message, wParam, lParam);
    }
	
	void UForm::Show(EShowMode ShowMode, EShowPosition ShowPosition)
	{
        if (!WindowHandle)
            CreateWin32Window();

		if (ShowPosition == EShowPosition::CenterScreen)
		{
            HMONITOR hMonitor = ::MonitorFromWindow(HWND(WindowHandle), 0);
			if (hMonitor)
			{
                MONITORINFO MonitorInfo = { sizeof(MONITORINFO) };
                GetMonitorInfoW(hMonitor, &MonitorInfo);

                SizeF WindowSize = GetSize();
                PointF WindowPosition = 
                {
                    MonitorInfo.rcMonitor.left + (MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left - WindowSize.Width) * 0.5f,
                    MonitorInfo.rcMonitor.top + (MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top - WindowSize.Height) * 0.5f,
                };

                SetWindowPos(HWND(WindowHandle), NULL, int(WindowPosition.X), int(WindowPosition.Y), 0, 0, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);
			}
		}
		
        switch (ShowMode)
        {
        case EShowMode::Hide:
        {
            ::ShowWindow((HWND)WindowHandle, SW_HIDE);
            break;
        }
        case EShowMode::NoActive:
        {
            ::ShowWindow((HWND)WindowHandle, SW_SHOWNOACTIVATE);
            break;
        }
        case EShowMode::Default:
        {
            ::ShowWindow((HWND)WindowHandle, SW_SHOWNORMAL);
            break;
        }
        case EShowMode::Minimize:
        {
            ::ShowWindow((HWND)WindowHandle, SW_SHOWMINIMIZED);
            break;
        }
        case EShowMode::Maximize:
        {
            ::ShowWindow((HWND)WindowHandle, SW_SHOWMAXIMIZED);
            break;
        }
        default:
            break;
        }
        UpdateWindow((HWND)WindowHandle);
	}

    void UForm::CreateWin32Window()
    {
        HINSTANCE hInstance = Win32::Instance();

        WNDCLASSEXW WndClass = { sizeof(WNDCLASSEXW) };
        if (!GetClassInfoExW(hInstance, Win32::WINDOW_CLASS_NAME, &WndClass))
        {
            WndClass.cbSize = sizeof(WNDCLASSEXW);
            WndClass.style = 0;
            WndClass.lpfnWndProc = DefaultWndProc;
            WndClass.cbClsExtra = 0;
            WndClass.cbWndExtra = 0;
            WndClass.hInstance = hInstance;
            WndClass.hIcon = NULL;
            WndClass.hCursor = /*::LoadCursor(NULL, IDC_ARROW)*/NULL;
            WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
            WndClass.lpszMenuName = NULL;
            WndClass.lpszClassName = Win32::WINDOW_CLASS_NAME;
            WndClass.hIconSm = NULL;

            RegisterClassExW(&WndClass);
        }
        else
        {
        }

        auto [Style, StyleEx] = MakeWindowStyle(FormStyle, FormFlags);
        FWStringView TitleW = Strings::ToWide(Title);

        PointF WindowPosition = GetPosition();
        SizeF WindowSize = Size;
    	
        __CurrentForm = this;
        HWND hWnd = CreateWindowExW(
            StyleEx, Win32::WINDOW_CLASS_NAME, TitleW.Data, Style /*| WS_BORDER | WS_DLGFRAME*/,
            RoundI32(WindowPosition.X), RoundI32(WindowPosition.Y), RoundI32(WindowSize.Width), RoundI32(WindowSize.Height),
            NULL, NULL, hInstance, this);

        WindowHandle = hWnd;

    	if (!(FormFlags * EFormFlags::CustomDraw))
    	{
            FGraphicsService * GraphicsService = GApplication->GetService<FGraphicsService>();
            if (!GraphicsService)
                LogWarning(Str("Can not get GraphicsService"));
            else
                Graphics = GraphicsService->CreateGraphics(WindowHandle);
    	}
    }

    intx_t UForm::HandleMessage(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
#define CASE_MSG(M, F) case M: return F(Message, wParam, lParam)
        switch (Message)
        {
            CASE_MSG(WM_NCHITTEST, OnWmHitTest);
            CASE_MSG(WM_NCCALCSIZE, OnWmNcCalcSize);
            CASE_MSG(WM_SHOWWINDOW, OnWmShow);
            CASE_MSG(WM_ERASEBKGND, OnWmEraseBack);
            CASE_MSG(WM_PAINT, OnWmPaint);
            CASE_MSG(WM_NCPAINT, OnWmNcPaint);
            CASE_MSG(WM_NCACTIVATE, OnWmNcActivate);

            CASE_MSG(WM_MOUSEMOVE, OnWmMouseMove);
            CASE_MSG(WM_MOUSELEAVE, OnWmMouseLeave);

            CASE_MSG(WM_LBUTTONDOWN, OnWmMouseDownL);
            CASE_MSG(WM_LBUTTONUP, OnWmMouseUpL);

            CASE_MSG(WM_RBUTTONDOWN, OnWmMouseDownR);
            CASE_MSG(WM_RBUTTONUP, OnWmMouseUpR);

            //CASE_MSG(WM_MBUTTONDOWN, OnWmMouseDownM);
            //CASE_MSG(WM_MBUTTONUP, OnWmMouseUpM);

            CASE_MSG(WM_NCLBUTTONDOWN, OnWmNcMouseDownL);

            //CASE_MSG(WM_LBUTTONDBLCLK, OnWmMouseDBClick);
            CASE_MSG(WM_MOUSEWHEEL, OnWmMouseWheelV);

            CASE_MSG(WM_MOVE, OnWmMove);
            CASE_MSG(WM_SIZE, OnWmSize);
            //CASE_MSG(WM_SETTEXT, OnWmSetText);
            CASE_MSG(WM_ACTIVATE, OnWmActive);
            CASE_MSG(WM_SETFOCUS, OnWmSetFocus);
            CASE_MSG(WM_KILLFOCUS, OnWmKillFocus);
            CASE_MSG(WM_CHAR, OnWmChar);
            //CASE_MSG(WM_UNICHAR, OnWmChar);

            CASE_MSG(WM_SYSKEYDOWN, OnWmKeyDown);
            CASE_MSG(WM_KEYDOWN, OnWmKeyDown);
            CASE_MSG(WM_SYSKEYUP, OnWmKeyUp);
            CASE_MSG(WM_KEYUP, OnWmKeyUp);

            //CASE_MSG(WM_SETCURSOR, OnWmSetCursor);
            //CASE_MSG(WM_QUERYDRAGICON, OnWmQueryDrag);
            //CASE_MSG(WM_DROPFILES, OnWmDropFiles);
            //CASE_MSG(WM_NOTIFY, OnWmNotify);
            CASE_MSG(WM_CAPTURECHANGED, OnWmCaptureChanged);
            //CASE_MSG(WM_MOUSEACTIVATE, OnWmMouseActive);
            //CASE_MSG(WM_NCMOUSEMOVE, OnWmNcMouseMove);

            CASE_MSG(WM_CLOSE, OnWmClose);
            CASE_MSG(WM_DESTROY, OnWmDestroy);

            CASE_MSG(WM_GETMINMAXINFO, OnWmGetMinMaxInfo);
            CASE_MSG(WM_SYSCOMMAND, OnWmSysCommand);
            CASE_MSG(WM_WINDOWPOSCHANGING, OnWmWindowPosChanging);
            CASE_MSG(WM_WINDOWPOSCHANGED, OnWmWindowPosChanged);

        default:
            return OnDefault(Message, wParam, lParam);
        }
    	
        return 0;
    }

    intx_t UForm::OnDefault(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        return (intx_t)CallWindowProcW(DefWindowProc, (HWND)WindowHandle, (UINT)Message, (WPARAM)wParam, (LPARAM)lParam);
    }
	
    intx_t UForm::OnWmEraseBack(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
    	if (FormStyle == EFormStyle::System)
			return OnDefault(Message, wParam, lParam);
        return 0;
    }

    intx_t UForm::OnWmMove(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        //auto f = form();
        //if (!f)
        //    throw core::exception(core::e_nullptr);

        //f->setWindowPos(_pos().to<float32_t>());
        return 0;
    }

    intx_t UForm::OnWmSize(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        SizeU NewSize = LParamVec2U(lParam);
        SizeF NewSizeF = NewSize.To<float32_t>();
        Size = NewSizeF;
    	
        LayoutMeasure(NewSizeF);
        LayoutArrange(RectF(PointF(), DesiredSize));

        //RenderRect.Size = NewSize.To<float32_t>();
        if (Graphics)
            Graphics->Resize(NewSize);
        //auto size = _size();
        //// https://docs.microsoft.com/zh-cn/windows/desktop/Controls/cookbook-overview
        ////To avoid applying visual styles to a top level window, give the window a non-null region (SetWindowRgn). 
        ////The system assumes that a window with a non-NULL region is a specialized window that does not use visual styles.
        ////A child window associated with a non-visual-styles top level window may still apply visual styles even though the parent window does not.
        ////WINDOWPOS & wp = *reinterpret_cast<WINDOWPOS *>(lParam);
        ////if (_form_styles.any(ui::form_style::frameless))
        ////{
        ////    HRGN hrgn = CreateRectRgn(0, 0, size.cx, size.cy);
        ////    SetWindowRgn((HWND)_handle, hrgn, FALSE);
        ////}

        //if (_message_blocks.any(windowmessage_bock::wm_size))
        //    return OnDefault(WM_SIZE, wParam, lParam);

        //auto f = form();
        //if (!f)
        //    throw core::exception(core::e_nullptr);

        //if (wParam != SIZE_MINIMIZED)
        //    f->setWindowSize(size.to<float32_t>());
        //switch (wParam)
        //{
        //case SIZE_MINIMIZED: f->setFormState(ui::form_state::minimize); break;
        //case SIZE_MAXIMIZED: f->setFormState(ui::form_state::maximize); break;
        //case SIZE_RESTORED: f->setFormState(ui::form_state::normalize); break;
        //default: break;
        //}
        return 0;
    }

    intx_t UForm::OnWmShow(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        //if (!lParam)
        //{
        //    if (auto f = form())
        //        f->notifyWindowShown(!!wParam);
        //}
        return OnDefault(Message, wParam, lParam);
    }

    intx_t UForm::OnWmPaint(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        //return OnDefault(WM_PAINT, wParam, lParam);
        HWND hWnd = (HWND)WindowHandle;
        //if (!hwnd)
        //    return OnDefault(WM_PAINT, wParam, lParam);

        ////_render
        PAINTSTRUCT ps;
        HDC hdc = ::BeginPaint(hWnd, &ps);
        ::EndPaint(hWnd, &ps);


        if (!(FormFlags * EFormFlags::CustomDraw) && Graphics)
        {
            Graphics->Clear(Colors::None);
            Draw(*Graphics);
            Graphics->Flush();
        }
        //_render({ ps.rcPaint.left, ps.rcPaint.top,
        //    ps.rcPaint.right - ps.rcPaint.left,
        //    ps.rcPaint.bottom - ps.rcPaint.top });
        //return 0;
        return OnDefault(Message, wParam, lParam);
    }

    intx_t UForm::OnWmNcPaint(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        if (FormStyle == EFormStyle::System)
            return OnDefault(Message, wParam, lParam);
    	
        //return OnDefault(WM_NCPAINT, wParam, lParam);
        //auto f = form();
        //if (!f)
        //    throw core::exception(core::e_nullptr);
        //auto styles = f->styles();
        //if (styles.any(ui::form_style::frameless))
        //    return 0;

        return 0;
    }

    intx_t UForm::OnWmNcActivate(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        if (FormStyle == EFormStyle::System)
            return OnDefault(Message, wParam, lParam);
    	
        //HWND hwnd = (HWND)_handle;
        //if (!hwnd || ::IsIconic(hwnd))
        //    return OnDefault(WM_NCACTIVATE, wParam, lParam);

        //if (_form_styles.any(ui::form_style::frameless))
        //    return 1;
        return TRUE;

    }

    intx_t UForm::OnWmMouseMove(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        PointF Point = LParamVec2U(lParam).To<float32_t>();
        TReferPtr<UElement> Element = FindElementAt(Point);
    	if (Element)
    	{
            FMouseButtonEventArgs MouseButtonEventArgs(FMouse::PreviewMouseMove);
            MouseButtonEventArgs.Owner = Element.Get();
            MouseButtonEventArgs.Position = Point;
            Element->RaiseEvent(MouseButtonEventArgs);
    	}
    	
        //auto f = form();
        //if (!f)
        //    throw core::exception(core::e_nullptr);

        //_mouse_state.setWheelLines(0);
        //_mouse_state.setPos(core::pointi(core::i32li16((int32_t)lParam), core::i32hi16((int32_t)lParam)).to<float32_t>());
        //if (!_trackingMouse)
        //{
        //    //OnWmMouseEnter(uiMessage, wParam, lParam);
        //    TRACKMOUSEEVENT tme;
        //    tme.cbSize = sizeof(TRACKMOUSEEVENT);
        //    tme.dwFlags = TME_LEAVE;
        //    tme.dwHoverTime = 1;
        //    tme.hwndTrack = (HWND)_handle;
        //    TrackMouseEvent(&tme);
        //    _trackingMouse = true;
        //    _mouse_state.setHoving(true);
        //    f->notifyWindowMouse(_mouse_state, ui::mouse_button::EKeyCode::None, ui::mouse_action::enter);
        //}

        //f->notifyWindowMouse(_mouse_state, ui::mouse_button::EKeyCode::None, ui::mouse_action::move);
        return 0;
    }

    intx_t UForm::OnWmMouseLeave(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        //if (!_trackingMouse)
        //    return 0;
        //_trackingMouse = false;

        //auto f = form();
        //if (!f)
        //    throw core::exception(core::e_nullptr);

        //_mouse_state.setWheelLines(0);
        //_mouse_state.setButton(ui::mouse_button::mask, false);
        //_mouse_state.setHoving(false);
        //_mouse_state.setPos(core::pointi(core::i32li16((int32_t)lParam), core::i32hi16((int32_t)lParam)).to<float32_t>());
        //f->notifyWindowMouse(_mouse_state, ui::mouse_button::EKeyCode::None, ui::mouse_action::leave);
        //if (_cursor_context)
        //    _cursor_context->reset();
        return 0;
    }

    intx_t UForm::OnWmMouseDownL(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        PointF Point = LParamVec2U(lParam).To<float32_t>();
        TReferPtr<UElement> Element = FindElementAt(Point);
    	
        if (Element)
        {
            FMouseButtonEventArgs MouseButtonEventArgs(FMouse::PreviewMouseDown);
            MouseButtonEventArgs.Owner = Element.Get();
            MouseButtonEventArgs.Position = Point;
            Element->RaiseEvent(MouseButtonEventArgs);
        }
        //auto f = form();
        //if (!f)
        //    throw core::exception(core::e_nullptr);

        //_mouse_state.setWheelLines(0);
        //_mouse_state.setButton(ui::mouse_button::left, true);
        //_mouse_state.setPos(core::pointi(core::i32li16((int32_t)lParam), core::i32hi16((int32_t)lParam)).to<float32_t>());
        //f->notifyWindowMouse(_mouse_state, ui::mouse_button::left, ui::mouse_action::press);
        return 0;
    }

    intx_t UForm::OnWmMouseDownR(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        //auto f = form();
        //if (!f)
        //    throw core::exception(core::e_nullptr);

        //_mouse_state.setWheelLines(0);
        //_mouse_state.setButton(ui::mouse_button::right, true);
        //_mouse_state.setPos(core::pointi(core::i32li16((int32_t)lParam), core::i32hi16((int32_t)lParam)).to<float32_t>());
        //f->notifyWindowMouse(_mouse_state, ui::mouse_button::right, ui::mouse_action::press);
        return 0;
    }

    intx_t UForm::OnWmMouseUpL(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        PointF Point = LParamVec2U(lParam).To<float32_t>();
        TReferPtr<UElement> Element = FindElementAt(Point);
        if (Element)
        {
            FMouseButtonEventArgs MouseButtonEventArgs(FMouse::PreviewMouseUp);
            MouseButtonEventArgs.Owner = Element.Get();
            MouseButtonEventArgs.Position = Point;
            Element->RaiseEvent(MouseButtonEventArgs);
        }
    	
        //auto f = form();
        //if (!f)
        //    throw core::exception(core::e_nullptr);

        //_mouse_state.setWheelLines(0);
        //_mouse_state.setButton(ui::mouse_button::left, false);
        //_mouse_state.setPos(core::pointi(core::i32li16((int32_t)lParam), core::i32hi16((int32_t)lParam)).to<float32_t>());
        //f->notifyWindowMouse(_mouse_state, ui::mouse_button::left, ui::mouse_action::click);
        //f->notifyWindowMouse(_mouse_state, ui::mouse_button::left, ui::mouse_action::release);
        return 0;
    }

    intx_t UForm::OnWmMouseUpR(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        //auto f = form();
        //if (!f)
        //    throw core::exception(core::e_nullptr);

        //_mouse_state.setWheelLines(0);
        //_mouse_state.setButton(ui::mouse_button::right, false);
        //_mouse_state.setPos(core::pointi(core::i32li16((int32_t)lParam), core::i32hi16((int32_t)lParam)).to<float32_t>());
        //f->notifyWindowMouse(_mouse_state, ui::mouse_button::right, ui::mouse_action::click);
        //f->notifyWindowMouse(_mouse_state, ui::mouse_button::right, ui::mouse_action::release);
        return 0;
    }

    intx_t UForm::OnWmMouseWheelV(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        //HWND hwnd = (HWND)_handle;
        //if (!hwnd)
        //    return 0;

        //auto f = form();
        //if (!f)
        //    throw core::exception(core::e_nullptr);

        //POINT point = { core::i32li16((int32_t)lParam), core::i32hi16((int32_t)lParam) };
        //::ScreenToClient(hwnd, &point);
        ////core::pointi point = core::pointi(core::i32li16(lParam), core::i32hi16(lParam));
        ////core::pointi wheel = core::pointi(core::u32li16(wParam), core::u32hi16(wParam));
        //_mouse_state.setWheelLines(core::u32hi16((uint32_t)wParam) / WHEEL_DELTA);
        //_mouse_state.setPos(core::pointi(point.x, point.y).to<float32_t>());
        //f->notifyWindowMouse(_mouse_state, ui::mouse_button::EKeyCode::None, ui::mouse_action::wheel_v);
        return 0;
    }

    intx_t UForm::OnWmNcMouseDownL(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        return OnDefault(Message, wParam, lParam);
    }

    intx_t UForm::OnWmChar(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        //if (auto f = form())
        //    f->notifyWindowCharInput(char32_t(wParam));
        return 0;
    }

    intx_t UForm::OnWmKeyDown(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        //auto f = form();
        //if (!f)
        //    throw core::exception(core::e_nullptr);

		EKeyCode KeyCode = Win32VirtualKey2KeyCode(static_cast<uint32_t>(wParam));
        if (KeyCode == EKeyCode::A)
            PrintChildren();

        FSystem::DebugOutput(Format(Str("Key Down: {}"), KeyCode));
    	
        FKeyboardEventArgs KeyboardEventArgs(FMouse::PreviewMouseUp);
        KeyboardEventArgs.KeyCode = KeyCode;
    	
    	if (SelectedElement)
    	{
            KeyboardEventArgs.Owner = SelectedElement.Get();
            SelectedElement->RaiseEvent(KeyboardEventArgs);
    	}
        else
        {
            KeyboardEventArgs.Owner = this;
            this->RaiseEvent(KeyboardEventArgs);
        }
        //EKeyCode key = virtualkey2keycode(wParam);
        //_mouse_state.setKey(key, true);
        //f->notifyWindowKey(_mouse_state, key, ui::key_action::press);
        return 0;
    }

    intx_t UForm::OnWmKeyUp(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        //auto f = form();
        //if (!f)
        //    throw core::exception(core::e_nullptr);

        //EKeyCode key = virtualkey2keycode(wParam);
        //_mouse_state.setKey(key, false);
        //f->notifyWindowKey(_mouse_state, key, ui::key_action::release);
        return 0;
    }

    intx_t UForm::OnWmHitTest(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        if (FormStyle == EFormStyle::System)
        {
			intx_t Result = OnDefault(WM_NCHITTEST, wParam, lParam);
            if (Result != HTCLIENT)
                return Result;
        }

        POINT ScreenPoint = LParamPoint(lParam);
        ScreenToClient(static_cast<HWND>(WindowHandle), &ScreenPoint);
		TReferPtr<UElement> Element = FindElementAt(PointF(float32_t(ScreenPoint.x), float32_t(ScreenPoint.y)));
        return Element ? HTCLIENT : HTCAPTION;
    }

    intx_t UForm::OnWmCaptureChanged(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        //if (auto f = form())
        //    f->notifyWindowCaptured(_handle == GetCapture());
        return OnDefault(Message, wParam, lParam);
    }

    intx_t UForm::OnWmMouseActive(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        //if (auto f = form())
        //    f->notifyWindowCaptured(_handle == GetCapture());
        return OnDefault(Message, wParam, lParam);
    }

    intx_t UForm::OnWmActive(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        return OnDefault(Message, wParam, lParam);
    }
	
    intx_t UForm::OnWmSetFocus(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        //_mouse_state.setFocused(true);
        //if (auto f = form())
        //    f->notifyWindowFocused(_mouse_state, true);
        return OnDefault(Message, wParam, lParam);
    }

    intx_t UForm::OnWmKillFocus(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        //_mouse_state.setFocused(false);
        //_mouse_state.setAllKeys(false);
        //if (auto f = form())
        //    f->notifyWindowFocused(_mouse_state, false);
        return OnDefault(Message, wParam, lParam);
    }

    intx_t UForm::OnWmNcCalcSize(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        if (FormStyle == EFormStyle::System || !wParam)
            return OnDefault(Message, wParam, lParam);
    	
        NCCALCSIZE_PARAMS & NoneClientSize = *reinterpret_cast<NCCALCSIZE_PARAMS *>(lParam);
    	
        RECT rcClient = { 0, 0, NoneClientSize.rgrc[0].right - NoneClientSize.rgrc[0].left, NoneClientSize.rgrc[0].bottom - NoneClientSize.rgrc[0].top };
        NoneClientSize.rgrc[1] = NoneClientSize.rgrc[0];
        NoneClientSize.rgrc[2] = NoneClientSize.rgrc[1];
        NoneClientSize.rgrc[0] = rcClient;
        return 0;
    }

    intx_t UForm::OnWmGetMinMaxInfo(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        //HWND hwnd = (HWND)_handle;
        //auto f = form();
        //if (!hwnd || !f)
        //    return OnDefault(WM_GETMINMAXINFO, wParam, lParam);

        //if (_form_styles.any(ui::form_style::frameless))
        //{
        //    MINMAXINFO & info = *reinterpret_cast<MINMAXINFO *>(lParam);

        //    HMONITOR hm = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
        //    MONITORINFO mi = { sizeof(MONITORINFO) };
        //    ::GetMonitorInfoW(hm, &mi);
        //    info.ptMaxPosition = { mi.rcWork.left - mi.rcMonitor.left, mi.rcWork.top - mi.rcMonitor.top };
        //    info.ptMaxSize = { mi.rcWork.right - mi.rcWork.left, mi.rcWork.bottom - mi.rcWork.top };
        //    info.ptMaxTrackSize = info.ptMaxSize;
        //    auto & min = f->minSize();
        //    auto & max = f->maxSize();
        //    if (min.cx.avi())
        //        info.ptMinTrackSize.x = (int32_t)f->calc(min.cx);
        //    if (min.cy.avi())
        //        info.ptMinTrackSize.x = (int32_t)f->calc(min.cy);
        //    if (max.cx.avi())
        //        info.ptMaxTrackSize.x = (int32_t)f->calc(max.cx);
        //    if (max.cy.avi())
        //        info.ptMaxTrackSize.x = (int32_t)f->calc(max.cy);
        //    return OnDefault(Message, wParam, lParam);
        //}
        //else
            return OnDefault(Message, wParam, lParam);
    }

    intx_t UForm::OnWmSysCommand(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        return OnDefault(Message, wParam, lParam);
    }

    intx_t UForm::OnWmWindowPosChanging(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        return OnDefault(Message, wParam, lParam);
    }

    intx_t UForm::OnWmWindowPosChanged(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        return OnDefault(Message, wParam, lParam);
    }

    intx_t UForm::OnWmClose(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
		EReply Result = Closing.Excute();
        if (Result == EReply::Handled)
            return 0;

        Graphics.Reset();
        ::DestroyWindow((HWND)WindowHandle);
        return 0;
    }

    intx_t UForm::OnWmDestroy(uint32_t Message, uintx_t wParam, intx_t lParam)
    {
        if (GetFlagAll(FormFlags, EFormFlags::QuitOnClose))
            ::PostQuitMessage(0);

        return 0;
    }

    static const EKeyCode Win32VirtualKey2KeyCodeMap[0xff] =
    {
        //! 不可用的按键码。
        /*0x00*/EKeyCode::None,
        /*0x01*/EKeyCode::None, // vk_lbutton
        /*0x02*/EKeyCode::None, // vk_rbutton
        /*0x03*/EKeyCode::None, // vk_cancel
        /*0x04*/EKeyCode::None, // vm_mbutton
        /*0x05*/EKeyCode::None, // vk_xbutton1
        /*0x06*/EKeyCode::None, // vk_xbutton2
        /*0x07*/EKeyCode::None,
        /*0x08*/EKeyCode::BackSpace, // vk_back
        /*0x09*/EKeyCode::Tab, // vk_tab
        /*0x0a*/EKeyCode::None,
        /*0x0b*/EKeyCode::None,
        /*0x0c*/EKeyCode::Clear,
        /*0x0d*/EKeyCode::Enter,
        /*0x0e*/EKeyCode::None,
        /*0x0f*/EKeyCode::None,

        /*0x10*/EKeyCode::Shift,
        /*0x11*/EKeyCode::Ctrl,
        /*0x12*/EKeyCode::Alt,
        /*0x13*/EKeyCode::PauseBreak,
        /*0x14*/EKeyCode::Caps,
        /*0x15*/EKeyCode::ImeKana,
        /*0x16*/EKeyCode::ImeHangul,
        /*0x17*/EKeyCode::ImeJunja,
        /*0x18*/EKeyCode::ImeFinal,
        /*0x19*/EKeyCode::ImeKanji,
        /*0x1a*/EKeyCode::None,
        /*0x1b*/EKeyCode::Escape,
        /*0x1c*/EKeyCode::ImeConvert,
        /*0x1d*/EKeyCode::ImeNonconvert,
        /*0x1e*/EKeyCode::ImeAccept,
        /*0x1f*/EKeyCode::ImeModechange,

        /*0x20*/EKeyCode::Space,
        /*0x21*/EKeyCode::Pageup,
        /*0x22*/EKeyCode::Pagedown,
        /*0x23*/EKeyCode::End,
        /*0x24*/EKeyCode::Home,
        /*0x25*/EKeyCode::Left,
        /*0x26*/EKeyCode::Up,
        /*0x27*/EKeyCode::Right,
        /*0x28*/EKeyCode::Down,
        /*0x29*/EKeyCode::Select,
        /*0x2a*/EKeyCode::Print,
        /*0x2b*/EKeyCode::Excute,
        /*0x2c*/EKeyCode::PrintScreen,
        /*0x2d*/EKeyCode::Insert,
        /*0x2e*/EKeyCode::Del,
        /*0x2f*/EKeyCode::Help,

        /*0x30*/EKeyCode::Num0,
        /*0x31*/EKeyCode::Num1,
        /*0x32*/EKeyCode::Num2,
        /*0x33*/EKeyCode::Num3,
        /*0x34*/EKeyCode::Num4,
        /*0x35*/EKeyCode::Num5,
        /*0x36*/EKeyCode::Num6,
        /*0x37*/EKeyCode::Num7,
        /*0x38*/EKeyCode::Num8,
        /*0x39*/EKeyCode::Num9,
        /*0x3a*/EKeyCode::None,
        /*0x3b*/EKeyCode::None,
        /*0x3c*/EKeyCode::None,
        /*0x3d*/EKeyCode::None,
        /*0x3e*/EKeyCode::None,
        /*0x3f*/EKeyCode::None,

        /*0x40*/EKeyCode::None,
        /*0x41*/EKeyCode::A,
        /*0x42*/EKeyCode::B,
        /*0x43*/EKeyCode::C,
        /*0x44*/EKeyCode::D,
        /*0x45*/EKeyCode::E,
        /*0x46*/EKeyCode::F,
        /*0x47*/EKeyCode::G,
        /*0x48*/EKeyCode::H,
        /*0x49*/EKeyCode::I,
        /*0x4a*/EKeyCode::J,
        /*0x4b*/EKeyCode::K,
        /*0x4c*/EKeyCode::L,
        /*0x4d*/EKeyCode::M,
        /*0x4e*/EKeyCode::N,
        /*0x4f*/EKeyCode::O,
        /*0x50*/EKeyCode::P,
        /*0x51*/EKeyCode::Q,
        /*0x52*/EKeyCode::R,
        /*0x53*/EKeyCode::S,
        /*0x54*/EKeyCode::T,
        /*0x55*/EKeyCode::U,
        /*0x56*/EKeyCode::V,
        /*0x57*/EKeyCode::W,
        /*0x58*/EKeyCode::X,
        /*0x59*/EKeyCode::Y,
        /*0x5a*/EKeyCode::Z,
        /*0x5b*/EKeyCode::WinL,
        /*0x5c*/EKeyCode::WinR,
        /*0x5d*/EKeyCode::Apps,
        /*0x5e*/EKeyCode::None,
        /*0x5f*/EKeyCode::Sleep,

        /*0x60*/EKeyCode::Numpad0,
        /*0x61*/EKeyCode::Numpad1,
        /*0x62*/EKeyCode::Numpad2,
        /*0x63*/EKeyCode::Numpad3,
        /*0x64*/EKeyCode::Numpad4,
        /*0x65*/EKeyCode::Numpad5,
        /*0x66*/EKeyCode::Numpad6,
        /*0x67*/EKeyCode::Numpad7,
        /*0x68*/EKeyCode::Numpad8,
        /*0x69*/EKeyCode::Numpad9,
        /*0x6a*/EKeyCode::NumpadMul,
        /*0x6b*/EKeyCode::NumpadAdd,
        /*0x6c*/EKeyCode::NumpadEnter,
        /*0x6d*/EKeyCode::NumpadSub,
        /*0x6e*/EKeyCode::NumpadDecimal,
        /*0x6f*/EKeyCode::NumpadDiv,

        /*0x70*/EKeyCode::F1,
        /*0x71*/EKeyCode::F2,
        /*0x72*/EKeyCode::F3,
        /*0x73*/EKeyCode::F4,
        /*0x74*/EKeyCode::F5,
        /*0x75*/EKeyCode::F6,
        /*0x76*/EKeyCode::F7,
        /*0x77*/EKeyCode::F8,
        /*0x78*/EKeyCode::F9,
        /*0x79*/EKeyCode::F10,
        /*0x7a*/EKeyCode::F11,
        /*0x7b*/EKeyCode::F12,
        /*0x7c*/EKeyCode::F13,
        /*0x7d*/EKeyCode::F14,
        /*0x7e*/EKeyCode::F15,
        /*0x7f*/EKeyCode::F16,
        /*0x80*/EKeyCode::F17,
        /*0x81*/EKeyCode::F18,
        /*0x82*/EKeyCode::F19,
        /*0x83*/EKeyCode::F20,
        /*0x84*/EKeyCode::F21,
        /*0x85*/EKeyCode::F22,
        /*0x86*/EKeyCode::F23,
        /*0x87*/EKeyCode::F24,
        /*0x88*/EKeyCode::None,
        /*0x89*/EKeyCode::None,
        /*0x8a*/EKeyCode::None,
        /*0x8b*/EKeyCode::None,
        /*0x8c*/EKeyCode::None,
        /*0x8d*/EKeyCode::None,
        /*0x8e*/EKeyCode::None,
        /*0x8f*/EKeyCode::None,

        /*0x90*/EKeyCode::NumpadLock,
        /*0x91*/EKeyCode::ScrollLock,
        /*0x92*/EKeyCode::NumpadEqual,
        /*0x93*/EKeyCode::None,
        /*0x94*/EKeyCode::None,
        /*0x95*/EKeyCode::None,
        /*0x96*/EKeyCode::None,
        /*0x97*/EKeyCode::None,
        /*0x98*/EKeyCode::None,
        /*0x99*/EKeyCode::None,
        /*0x9a*/EKeyCode::None,
        /*0x9b*/EKeyCode::None,
        /*0x9c*/EKeyCode::None,
        /*0x9d*/EKeyCode::None,
        /*0x9e*/EKeyCode::None,
        /*0x9f*/EKeyCode::None,

        /*0xa0*/EKeyCode::ShiftL,
        /*0xa1*/EKeyCode::ShiftR,
        /*0xa2*/EKeyCode::CtrlL,
        /*0xa3*/EKeyCode::CtrlR,
        /*0xa4*/EKeyCode::AltL,
        /*0xa5*/EKeyCode::AltR,
        /*0xa6*/EKeyCode::BrowserBack,
        /*0xa7*/EKeyCode::BrowserForward,
        /*0xa8*/EKeyCode::BrowserForward,
        /*0xa9*/EKeyCode::BrowserStop,
        /*0xaa*/EKeyCode::BrowserSearch,
        /*0xab*/EKeyCode::BrowserFavorites,
        /*0xac*/EKeyCode::BrowserHome,
        /*0xad*/EKeyCode::VolumeMute,
        /*0xae*/EKeyCode::VolumeDown,
        /*0xaf*/EKeyCode::VolumeUp,

        /*0xb0*/EKeyCode::MediaNext,
        /*0xb1*/EKeyCode::MediaPrev,
        /*0xb2*/EKeyCode::MediaStop,
        /*0xb3*/EKeyCode::MediaPlaypause,
        /*0xb4*/EKeyCode::LaunchMail,
        /*0xb5*/EKeyCode::LaunchMediaSelect,
        /*0xb6*/EKeyCode::LaunchApp1,
        /*0xb7*/EKeyCode::LaunchApp2,
        /*0xb8*/EKeyCode::None,
        /*0xb9*/EKeyCode::None,
        /*0xba*/EKeyCode::SemiColon,
        /*0xbb*/EKeyCode::Equal,
        /*0xbc*/EKeyCode::Comma,
        /*0xbd*/EKeyCode::Sub,
        /*0xbe*/EKeyCode::Period,
        /*0xbf*/EKeyCode::Slash,

        /*0xc0*/EKeyCode::Grave,
        /*0xc1*/EKeyCode::Reserved,
        /*0xc2*/EKeyCode::Reserved,
        /*0xc3*/EKeyCode::Reserved,
        /*0xc4*/EKeyCode::Reserved,
        /*0xc5*/EKeyCode::Reserved,
        /*0xc6*/EKeyCode::Reserved,
        /*0xc7*/EKeyCode::Reserved,
        /*0xc8*/EKeyCode::Reserved,
        /*0xc9*/EKeyCode::Reserved,
        /*0xca*/EKeyCode::Reserved,
        /*0xcb*/EKeyCode::Reserved,
        /*0xcc*/EKeyCode::Reserved,
        /*0xcd*/EKeyCode::Reserved,
        /*0xce*/EKeyCode::Reserved,
        /*0xcf*/EKeyCode::Reserved,

        /*0xd0*/EKeyCode::Reserved,
        /*0xd1*/EKeyCode::Reserved,
        /*0xd2*/EKeyCode::Reserved,
        /*0xd3*/EKeyCode::Reserved,
        /*0xd4*/EKeyCode::Reserved,
        /*0xd5*/EKeyCode::Reserved,
        /*0xd6*/EKeyCode::Reserved,
        /*0xd7*/EKeyCode::Reserved,
        /*0xd8*/EKeyCode::UnAssigned,
        /*0xd9*/EKeyCode::UnAssigned,
        /*0xda*/EKeyCode::UnAssigned,
        /*0xdb*/EKeyCode::BracketL,
        /*0xdc*/EKeyCode::BackSlash,
        /*0xdd*/EKeyCode::BracketR,
        /*0xde*/EKeyCode::Apostrophe,
        /*0xdf*/EKeyCode::None,

        /*0xe0*/EKeyCode::Reserved,
        /*0xe1*/EKeyCode::NotMatch,
        /*0xe2*/EKeyCode::Comma,
        /*0xe3*/EKeyCode::NotMatch,
        /*0xe4*/EKeyCode::NotMatch,
        /*0xe5*/EKeyCode::NotMatch,
        /*0xe6*/EKeyCode::NotMatch,
        /*0xe7*/EKeyCode::NotMatch,
        /*0xe8*/EKeyCode::NotMatch,
        /*0xe9*/EKeyCode::NotMatch,
        /*0xea*/EKeyCode::NotMatch,
        /*0xeb*/EKeyCode::NotMatch,
        /*0xec*/EKeyCode::NotMatch,
        /*0xed*/EKeyCode::NotMatch,
        /*0xee*/EKeyCode::NotMatch,
        /*0xef*/EKeyCode::NotMatch,

        /*0xf0*/EKeyCode::NotMatch,
        /*0xf1*/EKeyCode::NotMatch,
        /*0xf2*/EKeyCode::NotMatch,
        /*0xf3*/EKeyCode::NotMatch,
        /*0xf4*/EKeyCode::NotMatch,
        /*0xf5*/EKeyCode::NotMatch,
        /*0xf6*/EKeyCode::NotMatch,
        /*0xf7*/EKeyCode::NotMatch,
        /*0xf8*/EKeyCode::NotMatch,
        /*0xf9*/EKeyCode::NotMatch,
        /*0xfa*/EKeyCode::NotMatch,
        /*0xfb*/EKeyCode::NotMatch,
        /*0xfc*/EKeyCode::NotMatch,
        /*0xfd*/EKeyCode::NotMatch,
        /*0xfe*/EKeyCode::NotMatch,
        /*0xff*/
    };
	
    EKeyCode UForm::Win32VirtualKey2KeyCode(uint32_t VirtualKey)
    {
        if (VirtualKey >= 0xff)
            return EKeyCode::None;
    	return Win32VirtualKey2KeyCodeMap[VirtualKey];
    }

    uint32_t UForm::KeyCode2Win32VirtualKey(EKeyCode KeyCode)
    {
	    for (uint32_t VirtualKey = 0; VirtualKey < XE::Size(Win32VirtualKey2KeyCodeMap); ++VirtualKey)
	    {
            if (Win32VirtualKey2KeyCodeMap[VirtualKey] == KeyCode)
                return VirtualKey;
	    }
        return 0;
    }
}
#endif
