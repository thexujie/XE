#pragma once

#include "Core/Core.h"
#include "Graphics/Graphics.h"

#if XE_WINDOWS
#ifdef XEngine_UI_Module
#define UI_API __declspec(dllexport)
#else
#define UI_API __declspec(dllimport)
#endif
#endif

#include "Keyboard.h"

namespace XE::UI
{
	class UControl;
	
	enum class EShowMode
	{
		Default = 0,
		NoActive,
		Minimize,
		Maximize,
		Hide,
	};

	enum class EHitResult
	{
		None = 0,
		Client,
		Caption,
		ResizeLeftTop,
		ResizeTop,
		ResizeRightTop,
		ResizeRight,
		ResizeRightBottom,
		ResizeBottom,
		ResizeLeftBottom,
		ResizeLeft,
		Close,
		Minimize,
		Maximize,
	};

	enum class EHorizontalAlignment
	{
		Left = 0,
		Center,
		Right,
		Stretch,
	};
	
	enum class EVerticalAlignment
	{
		Top = 0,
		Center,
		Bottom,
		Stretch,
	};

	enum class EOrientation
	{
		Horizontal = 0,
		Vertical
	};

	enum class EUIObjectFlags
	{
		None = 0,
		Initialized = 0x0001,
	};
	template struct TFlag<EUIObjectFlags>;
	
	class UI_API FUIObject : public FChangable
	{
	public:
		FUIObject() = default;
		virtual ~FUIObject() = default;

		void SetUIObjectFlag(EUIObjectFlags UIObjectFlag, bool Set = true) { SetFlag(ObjectFlags, UIObjectFlag, Set); }
		bool IsUIObjectFlag(EUIObjectFlags UIObjectFlag) const { return GetFlag(ObjectFlags, UIObjectFlag); }
		
	protected:
		EUIObjectFlags ObjectFlags = EUIObjectFlags::None;
	};

	class UI_API IXuiService : public IService
	{
	public:
	};

	UI_API FStringView GetKeyCodeName(EKeyCode KeyCode);
}

namespace XE
{
	template<>
	class TFormatter<UI::EKeyCode>
	{
	public:
		FStringView Format(const UI::EKeyCode & KeyCode, FStringView Formal = FStringView::Empty) const { return UI::GetKeyCodeName(KeyCode); }
	};
}
