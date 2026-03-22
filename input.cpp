#include "pch.h"

#include "input.h"

namespace Input
{
	struct KEY_STATE
	{
		DWORD Key = 0;
		LPCSTR Name = NULL;
		BOOL Held = FALSE;
		BOOL Toggled = FALSE;
	};

	static KEY_STATE KeyStates[] = {
		{}, // 0x00
		{ VK_LBUTTON, "LBUTTON" },
		{ VK_RBUTTON, "RBUTTON" },
		{ VK_CANCEL, "CANCEL" },
		{ VK_MBUTTON, "MBUTTON" },
		{ VK_XBUTTON1, "XBUTTON1" },
		{ VK_XBUTTON2, "XBUTTON2" },
		{}, // 0x07
		{ VK_BACK, "BACK" },
		{ VK_TAB, "TAB" },
		{}, // 0x0A
		{}, // 0x0B
		{ VK_CLEAR, "CLEAR" },
		{ VK_RETURN, "RETURN" },
		{}, // 0x0E
		{}, // 0x0F
		{ VK_SHIFT, "SHIFT" },
		{ VK_CONTROL, "CONTROL" },
		{ VK_MENU, "MENU" },
		{ VK_PAUSE, "PAUSE" },
		{ VK_CAPITAL, "CAPITAL" },
		{ VK_KANA, "KANA" },
		{ VK_IME_ON, "IME_ON" },
		{ VK_JUNJA, "JUNJA" },
		{ VK_FINAL, "FINAL" },
		{ VK_HANJA, "HANJA" },
		{ VK_IME_OFF, "IME_OFF" },
		{ VK_ESCAPE, "ESCAPE" },
		{ VK_CONVERT, "CONVERT" },
		{ VK_NONCONVERT, "NONCONVERT" },
		{ VK_ACCEPT, "ACCEPT" },
		{ VK_MODECHANGE, "MODECHANGE" },
		{ VK_SPACE, "SPACE" },
		{ VK_PRIOR, "PRIOR" },
		{ VK_NEXT, "NEXT" },
		{ VK_END, "END" },
		{ VK_HOME, "HOME" },
		{ VK_LEFT, "LEFT" },
		{ VK_UP, "UP" },
		{ VK_RIGHT, "RIGHT" },
		{ VK_DOWN, "DOWN" },
		{ VK_SELECT, "SELECT" },
		{ VK_PRINT, "PRINT" },
		{ VK_EXECUTE, "EXECUTE" },
		{ VK_SNAPSHOT, "SNAPSHOT" },
		{ VK_INSERT, "INSERT" },
		{ VK_DELETE, "DELETE" },
		{ VK_HELP, "HELP" },
		{ '0', "0" },
		{ '1', "1" },
		{ '2', "2" },
		{ '3', "3" },
		{ '4', "4" },
		{ '5', "5" },
		{ '6', "6" },
		{ '7', "7" },
		{ '8', "8" },
		{ '9', "9" },
		{}, // 0x3A
		{}, // 0x3B
		{}, // 0x3C
		{}, // 0x3D
		{}, // 0x3E
		{}, // 0x3F
		{}, // 0x40
		{ 'A', "A" },
		{ 'B', "B" },
		{ 'C', "C" },
		{ 'D', "D" },
		{ 'E', "E" },
		{ 'F', "F" },
		{ 'G', "G" },
		{ 'H', "H" },
		{ 'I', "I" },
		{ 'J', "J" },
		{ 'K', "K" },
		{ 'L', "L" },
		{ 'M', "M" },
		{ 'N', "N" },
		{ 'O', "O" },
		{ 'P', "P" },
		{ 'Q', "Q" },
		{ 'R', "R" },
		{ 'S', "S" },
		{ 'T', "T" },
		{ 'U', "U" },
		{ 'V', "V" },
		{ 'W', "W" },
		{ 'X', "X" },
		{ 'Y', "Y" },
		{ 'Z', "Z" },
		{ VK_LWIN, "LWIN" },
		{ VK_RWIN, "RWIN" },
		{ VK_APPS, "APPS" },
		{}, // 0x5E
		{ VK_SLEEP, "SLEEP" },
		{ VK_NUMPAD0, "NUMPAD0" },
		{ VK_NUMPAD1, "NUMPAD1" },
		{ VK_NUMPAD2, "NUMPAD2" },
		{ VK_NUMPAD3, "NUMPAD3" },
		{ VK_NUMPAD4, "NUMPAD4" },
		{ VK_NUMPAD5, "NUMPAD5" },
		{ VK_NUMPAD6, "NUMPAD6" },
		{ VK_NUMPAD7, "NUMPAD7" },
		{ VK_NUMPAD8, "NUMPAD8" },
		{ VK_NUMPAD9, "NUMPAD9" },
		{ VK_MULTIPLY, "MULTIPLY" },
		{ VK_ADD, "ADD" },
		{ VK_SEPARATOR, "SEPARATOR" },
		{ VK_SUBTRACT, "SUBTRACT" },
		{ VK_DECIMAL, "DECIMAL" },
		{ VK_DIVIDE, "DIVIDE" },
		{ VK_F1, "F1" },
		{ VK_F2, "F2" },
		{ VK_F3, "F3" },
		{ VK_F4, "F4" },
		{ VK_F5, "F5" },
		{ VK_F6, "F6" },
		{ VK_F7, "F7" },
		{ VK_F8, "F8" },
		{ VK_F9, "F9" },
		{ VK_F10, "F10" },
		{ VK_F11, "F11" },
		{ VK_F12, "F12" },
		{ VK_F13, "F13" },
		{ VK_F14, "F14" },
		{ VK_F15, "F15" },
		{ VK_F16, "F16" },
		{ VK_F17, "F17" },
		{ VK_F18, "F18" },
		{ VK_F19, "F19" },
		{ VK_F20, "F20" },
		{ VK_F21, "F21" },
		{ VK_F22, "F22" },
		{ VK_F23, "F23" },
		{ VK_F24, "F24" },
		{ VK_NAVIGATION_VIEW, "NAVIGATION_VIEW" },
		{ VK_NAVIGATION_MENU, "NAVIGATION_MENU" },
		{ VK_NAVIGATION_UP, "NAVIGATION_UP" },
		{ VK_NAVIGATION_DOWN, "NAVIGATION_DOWN" },
		{ VK_NAVIGATION_LEFT, "NAVIGATION_LEFT" },
		{ VK_NAVIGATION_RIGHT, "NAVIGATION_RIGHT" },
		{ VK_NAVIGATION_ACCEPT, "NAVIGATION_ACCEPT" },
		{ VK_NAVIGATION_CANCEL, "NAVIGATION_CANCEL" },
		{ VK_NUMLOCK, "NUMLOCK" },
		{ VK_SCROLL, "SCROLL" },
		{ VK_OEM_NEC_EQUAL, "OEM_NEC_EQUAL" },
		{ VK_OEM_FJ_MASSHOU, "OEM_FJ_MASSHOU" },
		{ VK_OEM_FJ_TOUROKU, "OEM_FJ_TOUROKU" },
		{ VK_OEM_FJ_LOYA, "OEM_FJ_LOYA" },
		{ VK_OEM_FJ_ROYA, "OEM_FJ_ROYA" },
		{}, // 0x97
		{}, // 0x98
		{}, // 0x99
		{}, // 0x9A
		{}, // 0x9B
		{}, // 0x9C
		{}, // 0x9D
		{}, // 0x9E
		{}, // 0x9F
		{ VK_LSHIFT, "LSHIFT" },
		{ VK_RSHIFT, "RSHIFT" },
		{ VK_LCONTROL, "LCONTROL" },
		{ VK_RCONTROL, "RCONTROL" },
		{ VK_LMENU, "LMENU" },
		{ VK_RMENU, "RMENU" },
		{ VK_BROWSER_BACK, "BROWSER_BACK" },
		{ VK_BROWSER_FORWARD, "BROWSER_FORWARD" },
		{ VK_BROWSER_REFRESH, "BROWSER_REFRESH" },
		{ VK_BROWSER_STOP, "BROWSER_STOP" },
		{ VK_BROWSER_SEARCH, "BROWSER_SEARCH" },
		{ VK_BROWSER_FAVORITES, "BROWSER_FAVORITES" },
		{ VK_BROWSER_HOME, "BROWSER_HOME" },
		{ VK_VOLUME_MUTE, "VOLUME_MUTE" },
		{ VK_VOLUME_DOWN, "VOLUME_DOWN" },
		{ VK_VOLUME_UP, "VOLUME_UP" },
		{ VK_MEDIA_NEXT_TRACK, "MEDIA_NEXT_TRACK" },
		{ VK_MEDIA_PREV_TRACK, "MEDIA_PREV_TRACK" },
		{ VK_MEDIA_STOP, "MEDIA_STOP" },
		{ VK_MEDIA_PLAY_PAUSE, "MEDIA_PLAY_PAUSE" },
		{ VK_LAUNCH_MAIL, "LAUNCH_MAIL" },
		{ VK_LAUNCH_MEDIA_SELECT, "LAUNCH_MEDIA_SELECT" },
		{ VK_LAUNCH_APP1, "LAUNCH_APP1" },
		{ VK_LAUNCH_APP2, "LAUNCH_APP2" },
		{}, // 0xB8
		{}, // 0xB9
		{ VK_OEM_1, "OEM_1" },
		{ VK_OEM_PLUS, "OEM_PLUS" },
		{ VK_OEM_COMMA, "OEM_COMMA" },
		{ VK_OEM_MINUS, "OEM_MINUS" },
		{ VK_OEM_PERIOD, "OEM_PERIOD" },
		{ VK_OEM_2, "OEM_2" },
		{ VK_OEM_3, "OEM_3" },
		{}, // 0xC1
		{}, // 0xC2
		{ VK_GAMEPAD_A, "GAMEPAD_A" },
		{ VK_GAMEPAD_B, "GAMEPAD_B" },
		{ VK_GAMEPAD_X, "GAMEPAD_X" },
		{ VK_GAMEPAD_Y, "GAMEPAD_Y" },
		{ VK_GAMEPAD_RIGHT_SHOULDER, "GAMEPAD_RIGHT_SHOULDER" },
		{ VK_GAMEPAD_LEFT_SHOULDER, "GAMEPAD_LEFT_SHOULDER" },
		{ VK_GAMEPAD_LEFT_TRIGGER, "GAMEPAD_LEFT_TRIGGER" },
		{ VK_GAMEPAD_RIGHT_TRIGGER, "GAMEPAD_RIGHT_TRIGGER" },
		{ VK_GAMEPAD_DPAD_UP, "GAMEPAD_DPAD_UP" },
		{ VK_GAMEPAD_DPAD_DOWN, "GAMEPAD_DPAD_DOWN" },
		{ VK_GAMEPAD_DPAD_LEFT, "GAMEPAD_DPAD_LEFT" },
		{ VK_GAMEPAD_DPAD_RIGHT, "GAMEPAD_DPAD_RIGHT" },
		{ VK_GAMEPAD_MENU, "GAMEPAD_MENU" },
		{ VK_GAMEPAD_VIEW, "GAMEPAD_VIEW" },
		{ VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON, "GAMEPAD_LEFT_THUMBSTICK_BUTTON" },
		{ VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON, "GAMEPAD_RIGHT_THUMBSTICK_BUTTON" },
		{ VK_GAMEPAD_LEFT_THUMBSTICK_UP, "GAMEPAD_LEFT_THUMBSTICK_UP" },
		{ VK_GAMEPAD_LEFT_THUMBSTICK_DOWN, "GAMEPAD_LEFT_THUMBSTICK_DOWN" },
		{ VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT, "GAMEPAD_LEFT_THUMBSTICK_RIGHT" },
		{ VK_GAMEPAD_LEFT_THUMBSTICK_LEFT, "GAMEPAD_LEFT_THUMBSTICK_LEFT" },
		{ VK_GAMEPAD_RIGHT_THUMBSTICK_UP, "GAMEPAD_RIGHT_THUMBSTICK_UP" },
		{ VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN, "GAMEPAD_RIGHT_THUMBSTICK_DOWN" },
		{ VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT, "GAMEPAD_RIGHT_THUMBSTICK_RIGHT" },
		{ VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT, "GAMEPAD_RIGHT_THUMBSTICK_LEFT" },
		{ VK_OEM_4, "OEM_4" },
		{ VK_OEM_5, "OEM_5" },
		{ VK_OEM_6, "OEM_6" },
		{ VK_OEM_7, "OEM_7" },
		{ VK_OEM_8, "OEM_8" },
		{}, // 0xE0
		{ VK_OEM_AX, "OEM_AX" },
		{ VK_OEM_102, "OEM_102" },
		{ VK_ICO_HELP, "ICO_HELP" },
		{ VK_ICO_00, "ICO_00" },
		{ VK_PROCESSKEY, "PROCESSKEY" },
		{ VK_ICO_CLEAR, "ICO_CLEAR" },
		{ VK_PACKET, "PACKET" },
		{}, // 0xE8
		{ VK_OEM_RESET, "OEM_RESET" },
		{ VK_OEM_JUMP, "OEM_JUMP" },
		{ VK_OEM_PA1, "OEM_PA1" },
		{ VK_OEM_PA2, "OEM_PA2" },
		{ VK_OEM_PA3, "OEM_PA3" },
		{ VK_OEM_WSCTRL, "OEM_WSCTRL" },
		{ VK_OEM_CUSEL, "OEM_CUSEL" },
		{ VK_OEM_ATTN, "OEM_ATTN" },
		{ VK_OEM_FINISH, "OEM_FINISH" },
		{ VK_OEM_COPY, "OEM_COPY" },
		{ VK_OEM_AUTO, "OEM_AUTO" },
		{ VK_OEM_ENLW, "OEM_ENLW" },
		{ VK_OEM_BACKTAB, "OEM_BACKTAB" },
		{ VK_ATTN, "ATTN" },
		{ VK_CRSEL, "CRSEL" },
		{ VK_EXSEL, "EXSEL" },
		{ VK_EREOF, "EREOF" },
		{ VK_PLAY, "PLAY" },
		{ VK_ZOOM, "ZOOM" },
		{ VK_NONAME, "NONAME" },
		{ VK_PA1, "PA1" },
		{ VK_OEM_CLEAR, "OEM_CLEAR" },
		{} // 0xFF
	};

	static BYTE LastKeyDown = 0;

	static void UpdateMouseState(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_LBUTTONDOWN:
		{
			KEY_STATE& KeyState = KeyStates[VK_LBUTTON];
			KeyState.Held = TRUE;
			LastKeyDown = VK_LBUTTON;
			break;
		}
		case WM_LBUTTONUP:
		{
			KEY_STATE& KeyState = KeyStates[VK_LBUTTON];
			KeyState.Held = FALSE;
			KeyState.Toggled = !KeyState.Toggled;
			break;
		}
		case WM_RBUTTONDOWN:
		{
			KEY_STATE& KeyState = KeyStates[VK_RBUTTON];
			KeyState.Held = TRUE;
			LastKeyDown = VK_RBUTTON;
			break;
		}
		case WM_RBUTTONUP:
		{
			KEY_STATE& KeyState = KeyStates[VK_RBUTTON];
			KeyState.Held = FALSE;
			KeyState.Toggled = !KeyState.Toggled;
			break;
		}
		case WM_MBUTTONDOWN:
		{
			KEY_STATE& KeyState = KeyStates[VK_MBUTTON];
			KeyState.Held = TRUE;
			LastKeyDown = VK_MBUTTON;
			break;
		}
		case WM_MBUTTONUP:
		{
			KEY_STATE& KeyState = KeyStates[VK_MBUTTON];
			KeyState.Held = FALSE;
			KeyState.Toggled = !KeyState.Toggled;
			break;
		}
		case WM_XBUTTONDOWN:
		{
			KEY_STATE& KeyState = KeyStates[GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2];
			KeyState.Held = TRUE;
			LastKeyDown = GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2;
			break;
		}
		case WM_XBUTTONUP:
		{
			KEY_STATE& KeyState = KeyStates[GET_XBUTTON_WPARAM(wParam) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2];
			KeyState.Held = FALSE;
			KeyState.Toggled = !KeyState.Toggled;
			break;
		}
		default:
			break;
		}
	}

	static void UpdateKeyboardState(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			BYTE Key = wParam & 0xFF;
			KEY_STATE& KeyState = KeyStates[Key];
			KeyState.Held = TRUE;
			LastKeyDown = Key;
			break;
		}
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			BYTE Key = wParam & 0xFF;
			KEY_STATE& KeyState = KeyStates[Key];
			KeyState.Held = FALSE;
			KeyState.Toggled = !KeyState.Toggled;
			break;
		}
		default:
			break;
		}
	}

	void UpdateKeyState(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
			UpdateMouseState(uMsg, wParam, lParam);
			break;
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
			UpdateKeyboardState(uMsg, wParam, lParam);
			break;
		default:
			break;
		}
	}

	static KEY_STATE& GetKeyState(BYTE Key)
	{
		return KeyStates[Key];
	}

	BOOL GetKeyStateValue(BYTE Key, BOOL bHeld)
	{
		if (Key == 0x00 || Key == 0xFF)
		{
			return TRUE;
		}

		const KEY_STATE& State = GetKeyState(Key);

		return bHeld ? State.Held : State.Toggled;
	}

	LPCSTR GetKeyName(BYTE Key)
	{
		return GetKeyState(Key).Name;
	}

	BOOL IsKeyHeld(BYTE Key)
	{
		return GetKeyStateValue(Key, TRUE);
	}

	BOOL IsKeyToggled(BYTE Key)
	{
		return GetKeyStateValue(Key, FALSE);
	}

	void ClearLastKeyDown()
	{
		LastKeyDown = 0x00;
	}

	BYTE GetLastKeyDown()
	{
		return LastKeyDown;
	}
}
