#pragma once

#include "Core/Core.h"

namespace XE::UI
{
	enum class EKeyCode : uint8_t
	{
		//! 不可用的按键码。
		None = 0x0,

		// 数字键
		Num0,
		Num1,
		Num2,
		Num3,
		Num4,
		Num5,
		Num6,
		Num7,
		Num8,
		Num9,
		// 字符键
		Caps,
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,

		BackSpace,
		Tab,
		Enter,

		Escape,
		Space,

		// 小键盘
		NumpadLock,
		Numpad0,
		Numpad1,
		Numpad2,
		Numpad3,
		Numpad4,
		Numpad5,
		Numpad6,
		Numpad7,
		Numpad8,
		Numpad9,
		NumpadAdd, // +
		NumpadSub, // -
		NumpadMul, // *
		NumpadDiv,  // /
		NumpadDecimal, // .
		NumpadEnter, // enter
		NumpadEqual, // equal

		// 功能键
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24,

		// --  Sign
		Sub, // 减号 -_
		Equal, // 等于 =+
		Grave, // 抑音符 `~
		Comma, // 逗号 ，<
		Period, // 句号 .>
		Slash, // 斜杠 /?
		SemiColon, // 分号 ;:
		Colon, // 冒号 : (某些键盘有单独的冒号)
		Apostrophe, // 单引号、省字符 '
		BracketL, // 左方括号 [{
		BracketR, // 右方括号 ]}
		BackSlash, // 反斜杠 \|

		Insert,
		Del,
		Home,
		End,
		Pageup,
		Pagedown,

		Left,
		Up,
		Right,
		Down,

		PrintScreen,
		ScrollLock,
		PauseBreak,

		// -- win
		Win,
		WinL,
		WinR,
		// -- ctrl
		Ctrl,
		CtrlL,
		CtrlR,
		// -- shift
		Shift,
		ShiftL,
		ShiftR,
		// -- alt
		Alt,
		AltL,
		AltR,
		// -- app
		Apps,

		// 特殊键
		Sleep,

		Clear,
		Select,
		Print,
		Excute,
		Help,

		// -- ime
		ImeKana,
		ImeHangul,
		ImeJunja,
		ImeFinal,
		ImeHanja,
		ImeKanji,
		ImeConvert,
		ImeNonconvert,
		ImeAccept,
		ImeModechange,
		ImeProcesskey,

		// -- browser
		BrowserBack,
		BrowserForward,
		BrowserRefresh,
		BrowserStop,
		BrowserSearch,
		BrowserFavorites,
		BrowserHome,

		// -- volume
		//! 静音
		VolumeMute,
		VolumeDown,
		VolumeUp,

		// -- media
		MediaNext,
		MediaPrev,
		MediaStop,
		MediaPlaypause,

		// -- launch
		LaunchMail,
		LaunchMediaSelect,
		LaunchApp1,
		LaunchApp2,

		// -- 其他
		ElseAttn,
		ElseArsel,
		ElseExsel,
		ElseEreof,
		ElsePlay,
		ElseZoom,
		ElsePa1,
		ElseClear,

		Reserved,
		UnAssigned,
		NotMatch,
		Count,
	};
}
