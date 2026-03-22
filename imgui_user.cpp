#include "pch.h"

#include "imgui_user.h"

#include "imgui_internal.h"

namespace ImGui
{
	struct GroupPanelHeaderBounds
	{
		ImRect Left;
		ImRect Right;
	};

	static ImVector<GroupPanelHeaderBounds> GroupPanelStack;

	void BeginGroupPanel(const char* label, const ImVec2& size)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;

		const ImGuiID id = window->GetID(label);
		ImGui::PushID(id);

		const ImVec2 cursor_pos = window->DC.CursorPos;
		const ImVec2 item_spacing = g.Style.ItemSpacing;
		const float frame_height = ImGui::GetFrameHeight();

		ImGui::BeginGroup();

		ImVec2 effective_size = size;
		if (effective_size.x < 0.0f)
			effective_size.x = ImGui::GetContentRegionAvail().x;

		ImGui::Dummy(ImVec2(effective_size.x, 0.0f));

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		ImGui::Dummy(ImVec2(frame_height * 0.5f, 0.0f));
		ImGui::SameLine(0.0f, 0.0f);

		ImGui::BeginGroup();

		ImGui::Dummy(ImVec2(frame_height * 0.5f, 0.0f));
		ImGui::SameLine(0.0f, 0.0f);
		ImGui::TextUnformatted(label);

		ImRect left_rect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

		const ImVec2 right_max = ImVec2(cursor_pos.x + effective_size.x - frame_height * 0.5f, left_rect.Max.y);
		ImRect right_rect = ImRect(ImVec2(right_max.x, left_rect.Min.y), right_max);

		ImGui::SameLine(0.0f, 0.0f);
		ImGui::Dummy(ImVec2(0.0f, frame_height + item_spacing.y));

		ImGui::EndGroup();
		ImGui::PopStyleVar(2);

		const float side_padding = frame_height * 0.5f;
		window->ContentRegionRect.Max.x -= side_padding;
		window->WorkRect.Max.x -= side_padding;
		window->InnerRect.Max.x -= side_padding;
		window->Size.x -= side_padding;

		ImGui::PushItemWidth(ImMax(0.0f, ImGui::CalcItemWidth() - side_padding));

		GroupPanelStack.push_back({ left_rect, right_rect });
	}

	void EndGroupPanel()
	{
		ImGuiContext& g = *GImGui;

		GroupPanelHeaderBounds& header = GroupPanelStack.back();
		GroupPanelStack.pop_back();

		ImGui::PopItemWidth();

		const float side_padding = ImGui::GetFrameHeight() * 0.5f;
		ImGuiWindow* window = g.CurrentWindow;
		window->ContentRegionRect.Max.x += side_padding;
		window->WorkRect.Max.x += side_padding;
		window->InnerRect.Max.x += side_padding;
		window->Size.x += side_padding;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		ImGui::EndGroup();

		const float frame_height = ImGui::GetFrameHeight();
		const ImVec2 item_spacing = g.Style.ItemSpacing;

		ImGui::SameLine(0.0f, 0.0f);
		ImGui::Dummy(ImVec2(frame_height * 0.5f, 0.0f));
		ImGui::Dummy(ImVec2(0.0f, frame_height * 0.5f - item_spacing.y));

		ImGui::EndGroup();

		ImVec2 item_min = ImGui::GetItemRectMin();
		ImVec2 item_max = ImGui::GetItemRectMax();

		const ImVec2 half_frame = ImVec2(frame_height * 0.25f, frame_height * 0.5f);
		ImRect frame_rect(item_min + half_frame, item_max - ImVec2(half_frame.x, 0.0f));

		header.Left.Min.x -= item_spacing.x;
		header.Left.Max.x += item_spacing.x;

		const bool has_right_part = (header.Right.Min.x != header.Right.Max.x);
		if (has_right_part)
		{
			header.Right.Min.x -= item_spacing.x;
			header.Right.Max.x += item_spacing.x;
		}

		ImDrawList* draw_list = window->DrawList;
		const ImU32 border_color = ImGui::GetColorU32(ImGuiCol_Border);

		ImGui::PushClipRect(ImVec2(-FLT_MAX, -FLT_MAX), ImVec2(header.Left.Min.x, FLT_MAX), true);
		draw_list->AddRect(frame_rect.Min, frame_rect.Max, border_color, half_frame.x);
		ImGui::PopClipRect();

		ImGui::PushClipRect(ImVec2(header.Left.Max.x, -FLT_MAX), ImVec2(has_right_part ? header.Right.Min.x : FLT_MAX, FLT_MAX), true);
		draw_list->AddRect(frame_rect.Min, frame_rect.Max, border_color, half_frame.x);
		ImGui::PopClipRect();

		ImGui::PushClipRect(ImVec2(header.Left.Min.x, header.Left.Max.y), ImVec2(header.Left.Max.x, FLT_MAX), true);
		draw_list->AddRect(frame_rect.Min, frame_rect.Max, border_color, half_frame.x);
		ImGui::PopClipRect();

		if (has_right_part)
		{
			ImGui::PushClipRect(ImVec2(header.Right.Min.x, header.Right.Max.y), ImVec2(header.Right.Max.x, FLT_MAX), true);
			draw_list->AddRect(frame_rect.Min, frame_rect.Max, border_color, half_frame.x);
			ImGui::PopClipRect();

			ImGui::PushClipRect(ImVec2(header.Right.Max.x, -FLT_MAX), ImVec2(FLT_MAX, FLT_MAX), true);
			draw_list->AddRect(frame_rect.Min, frame_rect.Max, border_color, half_frame.x);
			ImGui::PopClipRect();
		}

		ImGui::PopStyleVar(2);
		ImGui::PopID();
	}

	void Hotkey(const char* label, unsigned char* p_key, bool* p_held)
	{
		ImGui::PushID(label);

		unsigned char key = *p_key;

		if (key == 0xFF)
		{
			unsigned char last_key_down = Input::GetLastKeyDown();

			if (last_key_down == VK_ESCAPE)
			{
				*p_key = 0x00;
			}
			else if (last_key_down != 0x00)
			{
				*p_key = last_key_down;
			}
		}

		const char* button_lable;

		if (key == 0x00)
		{
			button_lable = "NONE";
		}
		else if (key == 0xFF)
		{
			button_lable = "...";
		}
		else
		{
			button_lable = Input::GetKeyName(key);

			if (!button_lable)
			{
				button_lable = "UNKNOWN";
			}
		}

		if (Button(button_lable))
		{
			*p_key = 0xFF;
			Input::ClearLastKeyDown();
		}

		if (key != 0x00 && key != 0xFF && p_held)
		{
			ImGui::SameLine();

			if (Button(*p_held ? "HOLD" : "TOGGLE"))
			{
				*p_held = !*p_held;
			}
		}

		ImGui::PopID();
	}

	void SmallHotkey(const char* label, unsigned char* p_key, bool* p_held)
	{
		ImGui::PushID(label);

		unsigned char key = *p_key;

		if (key == 0xFF)
		{
			unsigned char last_key_down = Input::GetLastKeyDown();

			if (last_key_down == VK_ESCAPE)
			{
				*p_key = 0x00;
			}
			else if (last_key_down != 0x00)
			{
				*p_key = last_key_down;
			}
		}

		const char* button_lable;

		if (key == 0x00)
		{
			button_lable = "NONE";
		}
		else if (key == 0xFF)
		{
			button_lable = "...";
		}
		else
		{
			button_lable = Input::GetKeyName(key);

			if (!button_lable)
			{
				button_lable = "UNKNOWN";
			}
		}

		if (SmallButton(button_lable))
		{
			*p_key = 0xFF;
			Input::ClearLastKeyDown();
		}

		if (key != 0x00 && key != 0xFF && p_held)
		{
			ImGui::SameLine();

			if (SmallButton(*p_held ? "HOLD" : "TOGGLE"))
			{
				*p_held = !*p_held;
			}
		}

		ImGui::PopID();
	}
}
