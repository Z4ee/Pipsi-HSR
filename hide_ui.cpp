#include "pch.h"

#include "hide_ui.h"

namespace HideUi
{
	static float LastUpdateTime = 0.0f;

	static HOOK OnUpdateHook = {};

	static System::Void OnUpdateDetour(RPG::Client::RPGApplication* _this)
	{
		float CurrentTime = UnityEngine::Time::get_unscaledTime();

		if (CurrentTime - LastUpdateTime >= 0.1f)
		{
			UnityEngine::Camera** Camera = RPG::Client::GlobalVars::StaticGet_s_UICamera();

			if (Camera && *Camera)
			{
				(*Camera)->set_enabled(!Options.HideUi || !Input::GetKeyStateValue(Options.HideUiKey, Options.HideUiKeyHeld));
			}

			LastUpdateTime = CurrentTime;
		}

		CALL_ORIGINAL(OnUpdateHook, OnUpdateDetour, _this);
	}

	void Menu()
	{
		ImGui::BeginGroupPanel("Hide UI");

		ImGui::Checkbox("Enable", &Options.HideUi);
		ImGui::SameLine();
		ImGui::Hotkey("##HideUiHotkey", &Options.HideUiKey, &Options.HideUiKeyHeld);

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
		if (!IsHookActive(&OnUpdateHook) && !CreateHook(&OnUpdateHook, (PBYTE)hIl2Cpp + RPG_CLIENT_RPGAPPLICATION_ONUPDATE_OFFSET, OnUpdateDetour, FALSE))
		{
			return FALSE;
		}

		return TRUE;
	}
}
