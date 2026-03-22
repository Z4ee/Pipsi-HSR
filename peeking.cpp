#include "pch.h"

#include "peeking.h"

namespace Peeking
{
	static HOOK SetElevationDitherAlphaValueHook = {};

	static System::Void SetElevationDitherAlphaValueDetour(RPG::Client::BaseShaderPropertyTransition* _this, System::Single a1)
	{
		if (Options.Peeking)
		{
			a1 = 1.f;
		}

		CALL_ORIGINAL(SetElevationDitherAlphaValueHook, SetElevationDitherAlphaValueDetour, _this, a1);
	}

	HOOK SetDitherAlphaValueWithAnimationHook = {};

	static System::Boolean SetDitherAlphaValueWithAnimationDetour(RPG::Client::BaseShaderPropertyTransition* _this, System::Single a1, System::Single a2, System::Single a3, System::Action* a4, RPG::Client::DitherSourcePriority a5)
	{
		if (Options.Peeking)
		{
			a1 = 1.f;
			a2 = 0.f;
			a3 = 0.f;
		}

		return CALL_ORIGINAL(SetDitherAlphaValueWithAnimationHook, SetDitherAlphaValueWithAnimationDetour, _this, a1, a2, a3, a4, a5);
	}

	HOOK _OnTriggerCameraElevationDitherHook = {};

	static System::Void _OnTriggerCameraElevationDitherDetour(RPG::Client::AdventurePhase* _this, System::Object* param)
	{
		if (Options.Peeking)
		{
			return;
		}

		return CALL_ORIGINAL(_OnTriggerCameraElevationDitherHook, _OnTriggerCameraElevationDitherDetour, _this, param);
	}

	void Menu()
	{
		ImGui::BeginGroupPanel("Peeking");

		ImGui::Checkbox("Enable", &Options.Peeking);

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
		if (!IsHookActive(&SetElevationDitherAlphaValueHook) && !CreateHook(&SetElevationDitherAlphaValueHook, (PBYTE)hIl2Cpp + RPG_CLIENT_BASESHADERPROPERTYTRANSITION_SETELEVATIONDITHERALPHAVALUE_OFFSET, SetElevationDitherAlphaValueDetour, FALSE))
		{
			return FALSE;
		}

		if (!IsHookActive(&SetDitherAlphaValueWithAnimationHook) && !CreateHook(&SetDitherAlphaValueWithAnimationHook, (PBYTE)hIl2Cpp + RPG_CLIENT_BASESHADERPROPERTYTRANSITION_SETDITHERALPHAVALUEWITHANIMATION_OFFSET, SetDitherAlphaValueWithAnimationDetour, FALSE))
		{
			return FALSE;
		}

		if (!IsHookActive(&_OnTriggerCameraElevationDitherHook) && !CreateHook(&_OnTriggerCameraElevationDitherHook, (PBYTE)hIl2Cpp + RPG_CLIENT_ADVENTUREPHASE__ONTRIGGERCAMERAELEVATIONDITHER_OFFSET, _OnTriggerCameraElevationDitherDetour, FALSE))
		{
			return FALSE;
		}

		return TRUE;
	}
}
