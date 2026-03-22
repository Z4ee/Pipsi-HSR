#include "pch.h"

#include "global_speed_changer.h"

namespace GlobalSpeedChanger
{
	static HOOK set_timeScaleHook = {};

	static System::Void set_timeScaleDetour(System::Single value)
	{
		if (Options.GlobalSpeedChanger && Input::GetKeyStateValue(Options.GlobalSpeedChangerKey, Options.GlobalSpeedChangerKeyHeld))
		{
			RPG::Client::GamePhaseManager** GamePhaseManager = RPG::Client::GlobalVars::StaticGet_s_GamePhaseManager();

			if (GamePhaseManager && *GamePhaseManager)
			{
				RPG::Client::GamePhaseType GamePhaseType = (*GamePhaseManager)->_CurrentGamePhaseType_k__BackingField;

				switch (GamePhaseType)
				{
				case RPG::Client::GamePhaseType::BattleLineup:
				case RPG::Client::GamePhaseType::QABattleLineup:
				case RPG::Client::GamePhaseType::Battle:
				case RPG::Client::GamePhaseType::BattleNew:
					break;
				default:
					value = Options.GlobalSpeedChangerValue;
					break;
				}
			}
		}

		CALL_ORIGINAL(set_timeScaleHook, set_timeScaleDetour, value);
	}

	static HOOK OnUpdateHook = {};

	static System::Void OnUpdateDetour(RPG::Client::RPGApplication* _this)
	{
		if (Options.GlobalSpeedChanger && Input::GetKeyStateValue(Options.GlobalSpeedChangerKey, Options.GlobalSpeedChangerKeyHeld))
		{
			if (UnityEngine::Time::get_timeScale() != Options.GlobalSpeedChangerValue)
			{
				UnityEngine::Time::set_timeScale(Options.GlobalSpeedChangerValue);
			}
		}

		CALL_ORIGINAL(OnUpdateHook, OnUpdateDetour, _this);
	}

	void Menu()
	{
		ImGui::BeginGroupPanel("Global Speed Changer");

		ImGui::Checkbox("Enable", &Options.GlobalSpeedChanger);
		ImGui::SameLine();
		ImGui::Hotkey("##GlobalSpeedChangerHotkey", &Options.GlobalSpeedChangerKey, &Options.GlobalSpeedChangerKeyHeld);

		if (Options.GlobalSpeedChanger)
		{
			ImGui::Indent();

			ImGui::SliderFloat("Speed", &Options.GlobalSpeedChangerValue, 0.1f, 50.0f, "%.1f");

			ImGui::Unindent();
		}

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
		if (!IsHookActive(&set_timeScaleHook) && !CreateHook(&set_timeScaleHook, (PBYTE)hIl2Cpp + UNITYENGINE_TIME_SET_TIMESCALE_OFFSET, set_timeScaleDetour, TRUE))
		{
			return FALSE;
		}

		if (!IsHookActive(&OnUpdateHook) && !CreateHook(&OnUpdateHook, (PBYTE)hIl2Cpp + RPG_CLIENT_RPGAPPLICATION_ONUPDATE_OFFSET, OnUpdateDetour, FALSE))
		{
			return FALSE;
		}

		return TRUE;
	}
}
