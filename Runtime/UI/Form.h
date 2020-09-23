#pragma once

#include "UIInc.h"
#include "Control.h"
#include "Window.h"
#include "ContentControl.h"

namespace XE::UI
{
	enum class EFormStyle
	{
		Default = 0,
		System,
		Layered,
	};

	enum class EFormFlags
	{
		None = 0,
		NoMinimizeBox = 0x0001,
		NoMaximizeBox = 0x0002,
		NoResizeBorder = 0x0004,
		NoTaskButton = 0x0008,
		TopMose = 0x0010,

        QuitOnClose = 0x00010000,
        CustomDraw = 0x00020000,
	};
    template struct TFlag<EFormFlags>;

	enum class EShowPosition
	{
		Default = 0,
		CenterScreen,
	};
	
	class UI_API UForm : public UContentControl
	{
	public:
		UForm() = default;

		void SetTitle(FStringView TitleIn) { Title = TitleIn; }
		
		void Show(EShowMode ShowMode = EShowMode::Default, EShowPosition ShowPosition = EShowPosition::Default);

        void SetFormFlag(EFormFlags FormFlag, bool Set)
        {
            SetFlag(FormFlags, FormFlag, Set);
        }

    public:
        void OnRender(FGraphics & Graphics) const override;
		
	private:
		EFormStyle FormStyle = EFormStyle::System;
		EFormFlags FormFlags = EFormFlags::None;
		FString Title;
		UForm * Owner = nullptr;
        TSharedPtr<FGraphics> Graphics;

    public:
        TEvent<void> Closing;

    public:
        TReferPtr<UElement> SelectedElement;
        TReferPtr<UElement> FocusedElement;
		
    public:
        static FProperty TitleProperty;
        friend UI_API void Reflection(TNativeType<UForm> & NativeType);


    public:
        void SelectElement(TReferPtr<UElement> Element);
		
#ifdef XE_WINDOWS
		void CreateWin32Window();
        static EKeyCode Win32VirtualKey2KeyCode(uint32_t VirtualKey);
        static uint32_t KeyCode2Win32VirtualKey(EKeyCode KeyCode);

	public:
        ptr_t WindowHandle = nullptr;
		intx_t HandleMessage(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnDefault(uint32_t Message, uintx_t wParam, intx_t lParam);
		
        intx_t OnWmEraseBack(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmMove(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmSize(uint32_t Message, uintx_t wParam, intx_t lParam);

        intx_t OnWmShow(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmPaint(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmNcPaint(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmNcActivate(uint32_t Message, uintx_t wParam, intx_t lParam);

        intx_t OnWmMouseMove(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmMouseLeave(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmMouseDownL(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmMouseDownR(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmMouseUpL(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmMouseUpR(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmMouseWheelV(uint32_t Message, uintx_t wParam, intx_t lParam);

        intx_t OnWmNcMouseDownL(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmMouseDownM(uint32_t Message, uintx_t wParam, intx_t lParam) { return 0; }
        intx_t OnWmMouseUpM(uint32_t Message, uintx_t wParam, intx_t lParam) { return 0; }

        intx_t OnWmSetText(uint32_t Message, uintx_t wParam, intx_t lParam) { return 0; }
        intx_t OnWmActive(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmSetFocus(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmKillFocus(uint32_t Message, uintx_t wParam, intx_t lParam);

        intx_t OnWmChar(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmUnicodeChar(uint32_t Message, uintx_t wParam, intx_t lParam)
        {
            //if(wParam != UNICODE_NOCHAR)
            //	m_pControl->PreOnKeyInput((char_32)wParam);

            // utf16 窗口无法收到这个消息 
            // https://msdn.microsoft.com/en-us/library/windows/desktop/ms646288(v=vs.85).aspx
            // WM_UNICHAR 返回 true 可以拒绝 WM_CHAR.
            return 0;
        }
        intx_t OnWmKeyDown(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmKeyUp(uint32_t Message, uintx_t wParam, intx_t lParam);

        intx_t OnWmMouseDBClick(uint32_t Message, uintx_t wParam, intx_t lParam) { return 0; }

        intx_t OnWmSetCursor(uint32_t Message, uintx_t wParam, intx_t lParam) { return 0; }
        intx_t OnWmDropFiles(uint32_t Message, uintx_t wParam, intx_t lParam) { return 0; }
        intx_t OnWmQueryDrag(uint32_t Message, uintx_t wParam, intx_t lParam) { return 0; }

        intx_t OnWmHitTest(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmNotify(uint32_t Message, uintx_t wParam, intx_t lParam) { return 0; }

        intx_t OnWmClose(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmDestroy(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmCaptureChanged(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmMouseActive(uint32_t Message, uintx_t wParam, intx_t lParam);

        intx_t OnWmNcCalcSize(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmNcMouseMove(uint32_t Message, uintx_t wParam, intx_t lParam) { return 0; }
        intx_t OnWmGetMinMaxInfo(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmSysCommand(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmWindowPosChanging(uint32_t Message, uintx_t wParam, intx_t lParam);
        intx_t OnWmWindowPosChanged(uint32_t Message, uintx_t wParam, intx_t lParam);
#endif
	};
}
