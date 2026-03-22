#include "pch.h"

#include "fov_changer.h"

namespace FovChanger
{
	static HOOK set_fieldOfViewHook = {};

	static System::Void set_fieldOfViewDetour(UnityEngine::Camera* _this, System::Single value)
	{
		if (Options.FovChanger)
		{
			value = Options.FovChangerValue;
		}

		CALL_ORIGINAL(set_fieldOfViewHook, set_fieldOfViewDetour, _this, value);
	}

	void Menu()
	{
		ImGui::BeginGroupPanel("FOV Changer");

		ImGui::Checkbox("Enable", &Options.FovChanger);
		ImGui::SameLine();
		ImGui::Hotkey("##FovChangerHotkey", &Options.FovChangerKey, &Options.FovChangerKeyHeld);

		if (Options.FovChanger)
		{
			ImGui::Indent();

			ImGui::SliderFloat("Value", &Options.FovChangerValue, 0.0f, 360.0f);

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
		if (!IsHookActive(&set_fieldOfViewHook) && !CreateHook(&set_fieldOfViewHook, (PBYTE)hIl2Cpp + UNITYENGINE_CAMERA_SET_FIELDOFVIEW_OFFSET, set_fieldOfViewDetour, TRUE))
		{
			return FALSE;
		}

		return TRUE;
	}
}
