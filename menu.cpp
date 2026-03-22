#include "pch.h"

#include "menu.h"

#include "imgui_impl_win32.h"
#include "cheat.h"
#include "d3d11_overlay.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Menu
{
	static BOOL OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (hIl2Cpp)
		{
			Input::UpdateKeyState(uMsg, wParam, lParam);

			if (Input::IsKeyToggled(Options.MenuKey))
			{
				if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
				{
					return FALSE;
				}

				switch (uMsg)
				{
				case WM_MOVE:
				case WM_SIZE:
				case WM_ACTIVATE:
				case WM_SETFOCUS:
				case WM_KILLFOCUS:
				case WM_PAINT:
				case WM_ERASEBKGND:
				case WM_ACTIVATEAPP:
				case WM_GETMINMAXINFO:
				case WM_WINDOWPOSCHANGING:
				case WM_WINDOWPOSCHANGED:
				case WM_NCCALCSIZE:
				case WM_NCHITTEST:
				case WM_NCPAINT:
				case WM_NCACTIVATE:
				case WM_SIZING:
				case WM_MOVING:
					break;
				default:
					return FALSE;
				}
			}
		}

		return TRUE;
	}

	static void BeforeFrame()
	{
		Cheat::BeforeFrame();
	}

	static const char* TabNames[] =
	{
		"Adventure",
		"Battle",
		"Visuals",
		"Misc"
	};

	static int CurrentTab = 0;

	static void OnFrame()
	{
		if (Input::IsKeyToggled(Options.MenuKey))
		{
			if (ImGui::Begin("Pipsi-HSR | " __DATE__ " | https://discord.gg/tPKFCs4VbB | IF YOU BOUGHT THIS, YOU GOT SCAMMED"))
			{
				if (ImGui::BeginListBox("##MenuTabs", ImVec2(ImGui::GetWindowWidth() / 5.0f, -FLT_MIN)))
				{
					for (int i = 0; i < IM_ARRAYSIZE(TabNames); i++)
					{
						if (ImGui::Selectable(TabNames[i], CurrentTab == i))
						{
							CurrentTab = i;
						}
					}

					ImGui::EndListBox();
				}

				ImGui::SameLine();


				if (ImGui::BeginChild("##MenuTabContent", ImVec2(0.0f, -FLT_MIN), ImGuiChildFlags_Borders))
				{
					switch (CurrentTab)
					{
					case 0:
						Cheat::MenuAdventure();
						break;
					case 1:
						Cheat::MenuBattle();
						break;
					case 2:
						Cheat::MenuVisuals();
						break;
					case 3:
						Cheat::MenuMisc();
						break;
					default:
						break;
					}
				}

				ImGui::EndChild();
			}
		}

		Cheat::OnFrame();
	}

	bool Setup()
	{
		if (!D3D11_Overlay::Setup(OnWndProc, BeforeFrame, OnFrame))
		{
			return FALSE;
		}

		if (!Cheat::Setup())
		{
			return FALSE;
		}

		return TRUE;
	}
}
