#pragma once
#include <windows.h>

namespace Input
{
	void UpdateKeyState(UINT uMsg, WPARAM wParam, LPARAM lParam);

	BOOL GetKeyStateValue(BYTE Key, BOOL bHeld);

	LPCSTR GetKeyName(BYTE Key);
	BOOL IsKeyHeld(BYTE Key);
	BOOL IsKeyToggled(BYTE Key);

	void ClearLastKeyDown();
	BYTE GetLastKeyDown();
}
