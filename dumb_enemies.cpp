#include "pch.h"

#include "dumb_enemies.h"

namespace DumbEnemies
{
	static HOOK TickAlertValueHook = {};

	static System::Void TickAlertValueDetour(System::Single fElapsedTimeInSec, RPG::GameCore::GameEntity* target, PVOID detail)
	{
		if (Options.DumbEnemies)
		{
			return;
		}

		CALL_ORIGINAL(TickAlertValueHook, TickAlertValueDetour, fElapsedTimeInSec, target, detail);
	}

	void Menu()
	{
		ImGui::BeginGroupPanel("Dumb Enemies");

		ImGui::Checkbox("Enable", &Options.DumbEnemies);
		ImGui::SameLine();
		ImGui::Hotkey("##DumbEnemiesHotkey", &Options.DumbEnemiesKey, &Options.DumbEnemiesKeyHeld);

		ImGui::EndGroupPanel();
	}

	void BeforeFrame()
	{
	}

	void OnFrame()
	{
	}

	bool Setup()
	{
		if (!IsHookActive(&TickAlertValueHook) && !CreateHook(&TickAlertValueHook, (PBYTE)hIl2Cpp + RPG_GAMECORE_NPCCOMPONENT_TICKALERTVALUE_OFFSET, TickAlertValueDetour, FALSE))
		{
			return FALSE;
		}

		return TRUE;
	}
}
