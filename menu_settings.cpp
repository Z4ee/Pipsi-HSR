#include "pch.h"

#include "menu_settings.h"

namespace MenuSettings
{
	void Menu()
	{
		ImGui::BeginGroupPanel("Menu Settings");

		ImGui::Text("Menu Key:");
		ImGui::SameLine();
		ImGui::SmallHotkey("##MenuHotkey", &Options.MenuKey);

		if (Options.MenuKey == VK_LBUTTON)
		{
			Options.MenuKey = 0;
		}

		ImGui::Checkbox("Custom Menu Scale", &Options.MenuCustomScale);

		if (Options.MenuCustomScale)
		{
			ImGui::Indent();

			ImGui::SliderFloat("Value", &Options.MenuCustomScaleValue, 0.5f, 4.0f, "%.2f");

			ImGui::Unindent();
		}

		ImGui::EndGroupPanel();
	}

	void BeforeFrame()
	{
		float Scale = Options.MenuCustomScale ? Options.MenuCustomScaleValue : 1.0f;

		ImGuiStyle& Style = ImGui::GetStyle();

		if (Style.FontScaleDpi != Scale)
		{
			ImGuiStyle NewStyle;

			NewStyle.ScaleAllSizes(Scale);
			NewStyle.FontScaleDpi = Scale;

			Style = NewStyle;
		}
	}

	void OnFrame()
	{
	}

	bool Setup()
	{
		return TRUE;
	}
}
