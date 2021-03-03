#pragma once

#include "Core/Core.h"

namespace XE::UI
{
	enum class EKeyCode : uint8_t
	{
		//! �����õİ����롣
		None = 0x0,

		// ���ּ�
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
		// �ַ���
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

		// С����
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

		// ���ܼ�
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
		Sub, // ���� -_
		Equal, // ���� =+
		Grave, // ������ `~
		Comma, // ���� ��<
		Period, // ��� .>
		Slash, // б�� /?
		SemiColon, // �ֺ� ;:
		Colon, // ð�� : (ĳЩ�����е�����ð��)
		Apostrophe, // �����š�ʡ�ַ� '
		BracketL, // ������ [{
		BracketR, // �ҷ����� ]}
		BackSlash, // ��б�� \|

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

		// �����
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
		//! ����
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

		// -- ����
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
