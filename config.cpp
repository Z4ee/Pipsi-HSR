#include "pch.h"

#include "config.h"

namespace Config
{
	static void SaveConfig()
	{
		HANDLE hFile = CreateFileW(L"config.cfg", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			return;
		}

		WriteFile(hFile, &Options, sizeof(Options), NULL, NULL);

		CloseHandle(hFile);
	}

	static void LoadConfig()
	{
		HANDLE hFile = CreateFileW(L"config.cfg", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			return;
		}

		if (ReadFile(hFile, &Options, sizeof(Options), NULL, NULL))
		{
		}

		CloseHandle(hFile);
	}

	static HOOK NtTerminateProcessHook = {};

	static NTSTATUS NtTerminateProcessDetour(HANDLE ProcessHandle, NTSTATUS ExitStatus)
	{
		SaveConfig();

		return CALL_ORIGINAL(NtTerminateProcessHook, NtTerminateProcessDetour, ProcessHandle, ExitStatus);
	}

	void Menu()
	{
		ImGui::BeginGroupPanel("Config");

		if (ImGui::Button("Save"))
		{
			SaveConfig();
		}

		ImGui::SameLine();

		if (ImGui::Button("Load"))
		{
			LoadConfig();
		}

		ImGui::SameLine();

		if (ImGui::Button("Reset"))
		{
			Options = {};
		}

		ImGui::EndGroupPanel();
	}

	void BeforeFrame()
	{
		if (Input::IsKeyHeld(VK_HOME))
		{
			Options = {};
		}
	}

	void OnFrame()
	{
	}

	bool Setup()
	{
		LoadConfig();

		HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");

		if (!hNtdll)
		{
			return FALSE;
		}

		if (!IsHookActive(&NtTerminateProcessHook) && !CreateHook(&NtTerminateProcessHook, GetProcAddress(hNtdll, "NtTerminateProcess"), NtTerminateProcessDetour, FALSE))
		{
			return FALSE;
		}

		return TRUE;
	}
}
