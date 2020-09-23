#include "PCH.h"
#include "UIInc.h"

namespace XE::UI
{
	static const char * KeyCodeNames[static_cast<size_t>(EKeyCode::Count)] =
	{
		" ",
		"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
		"Caps",
		"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
		"BackSpace", "Tab", "Enter",
		"Escape", "Space",
		"NumpadLock", "Numpad0", "Numpad1", "Numpad2", "Numpad3", "Numpad4", "Numpad5", "Numpad6", "Numpad7", "Numpad8", "Numpad9",
		"Numpad+", "Numpad-", "Numpad*", "Numpad/", "Numpad.", "NumpadEnter", "Numpad=",
		"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
		"F13", "F14", "F15", "F16", "F17", "F18", "F19", "F20", "F21", "F22", "F23", "F24",
		"-", "=", "~", ",", ".", "/", ";", ":", "'", "[", "]", "\\",
		"Insert", "Delete", "Home", "End", "PageUp", "PageDown",
		"¡û", "¡ü", "¡ú", "¡ý",
		"PrintScreen", "ScrollLock", "PauseBreak",
		"Win", "WinL", "WinR",
		"Ctrl", "CtrlL", "CtrlR",
		"Shift", "ShiftL", "ShiftR",
		"Alt", "AltL", "AltR",
		"Apps",
		"Sleep",
		"Clear","Select","Print","Excute","Help",
		"ImeKana","ImeHangul","ImeJunja","ImeFinal","ImeHanja","ImeKanji","ImeConvert","ImeNonconvert","ImeAccept","ImeModechange","ImeProcesskey",
		"BrowserBack","BrowserForward","BrowserRefresh","BrowserStop","BrowserSearch","BrowserFavorites","BrowserHome",
		"VolumeMute","VolumeDown","VolumeUp",
		"MediaNext","MediaPrev","MediaStop","MediaPlaypause",
		"LaunchMail","LaunchMediaSelect","LaunchApp1","LaunchApp2",
		"ElseAttn","ElseArsel","ElseExsel","ElseEreof","ElsePlay","ElseZoom","ElsePa1","ElseClear",
		"Reserved","UnAssigned","NotMatch",
	};

	FStringView GetKeyCodeName(EKeyCode KeyCode)
	{
		return FStringView((const Char *)KeyCodeNames[static_cast<uintx_t>(KeyCode)]);
	}
}