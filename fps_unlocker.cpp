#include "pch.h"

#include "fps_unlocker.h"

namespace FpsUnlocker
{
	static float LastUpdateTime = 0.0f;

	static HOOK OnUpdateHook = {};

	static System::Void OnUpdateDetour(RPG::Client::RPGApplication* _this)
	{
		float CurrentTime = UnityEngine::Time::get_unscaledTime();

		if (CurrentTime - LastUpdateTime >= 0.1f)
		{
			if (Options.FpsUnlocker)
			{
				UnityEngine::Application::set_targetFrameRate(Options.FpsUnlockerValue);
			}

			LastUpdateTime = CurrentTime;
		}

		CALL_ORIGINAL(OnUpdateHook, OnUpdateDetour, _this);
	}

	void Menu()
	{
		ImGui::BeginGroupPanel("FPS Unlocker");

		ImGui::Checkbox("Enable", &Options.FpsUnlocker);

		if (Options.FpsUnlocker)
		{
			ImGui::Indent();

			ImGui::SliderInt("FPS Limit", &Options.FpsUnlockerValue, 1, 1000);

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
		if (!IsHookActive(&OnUpdateHook) && !CreateHook(&OnUpdateHook, (PBYTE)hIl2Cpp + RPG_CLIENT_RPGAPPLICATION_ONUPDATE_OFFSET, OnUpdateDetour, FALSE))
		{
			return FALSE;
		}

		return TRUE;
	}
}