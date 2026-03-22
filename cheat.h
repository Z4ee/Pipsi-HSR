#pragma once
#include <windows.h>

namespace Cheat
{
	void MenuAdventure();
	void MenuBattle();
	void MenuVisuals();
	void MenuMisc();

	void BeforeFrame();
	void OnFrame();

	bool Setup();
}
