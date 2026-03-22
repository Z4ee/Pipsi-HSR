#include "pch.h"
#include "fps_indicator.h"

namespace FpsIndicator
{
	void Menu()
	{
		ImGui::BeginGroupPanel("FPS Indicator");

		ImGui::Checkbox("Enable", &Options.FpsIndicator);

		ImGui::EndGroupPanel();
	}

	void BeforeFrame()
	{
	}

	void OnFrame()
	{
		if (ImGui::Begin("FPS Indicator", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus))
		{
			ImGui::Text("FPS: %.0f", ImGui::GetIO().Framerate);
		}

		ImGui::End();
	}

	bool Setup()
	{
		return TRUE;
	}
}
