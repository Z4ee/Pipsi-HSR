#include "pch.h"

#include "cheat.h"

/* Adventure */
#include "dumb_enemies.h"
#include "noclip.h"
#include "peeking.h"

/* Battle */
#include "battle_speed_changer.h"
#include "crit_indicator.h"

/* Visuals */
#include "fov_changer.h"
#include "fps_indicator.h"
#include "free_camera.h"
#include "hide_ui.h"

/* Misc */
#include "config.h"
#include "fps_unlocker.h"
#include "global_speed_changer.h"
#include "menu_settings.h"

namespace Cheat
{
	static HOOK LoadLibraryExWHook = {};

	static HMODULE LoadLibraryExWDetour(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
	{
		HMODULE hModule = CALL_ORIGINAL(LoadLibraryExWHook, LoadLibraryExWDetour, lpLibFileName, hFile, dwFlags);

		if (!hIl2Cpp)
		{
			if (hModule && hModule == GetModuleHandleW(L"telemetry.dll"))
			{
				hIl2Cpp = GetModuleHandleW(L"GameAssembly.dll");

				/* Adventure */
				DumbEnemies::Setup();
				Noclip::Setup();
				Peeking::Setup();

				/* Battle */
				BattleSpeedChanger::Setup();
				CritIndicator::Setup();

				/* Visuals */
				FovChanger::Setup();
				FpsIndicator::Setup();
				FreeCamera::Setup();
				HideUi::Setup();

				/* Misc */
				Config::Setup();
				FpsUnlocker::Setup();
				GlobalSpeedChanger::Setup();
				MenuSettings::Setup();
			}
		}

		return hModule;
	}

	void MenuAdventure()
	{
		DumbEnemies::Menu();
		Noclip::Menu();
		Peeking::Menu();
	}

	void MenuBattle()
	{
		BattleSpeedChanger::Menu();
		CritIndicator::Menu();
	}

	void MenuVisuals()
	{
		FovChanger::Menu();
		FpsIndicator::Menu();
		FreeCamera::Menu();
		HideUi::Menu();
	}

	void MenuMisc()
	{
		Config::Menu();
		FpsUnlocker::Menu();
		GlobalSpeedChanger::Menu();
		MenuSettings::Menu();
	}

	void BeforeFrame()
	{
		/* Adventure */
		DumbEnemies::BeforeFrame();
		Noclip::BeforeFrame();
		Peeking::BeforeFrame();

		/* Battle */
		BattleSpeedChanger::BeforeFrame();
		CritIndicator::BeforeFrame();

		/* Visuals */
		FovChanger::BeforeFrame();
		FpsIndicator::BeforeFrame();
		FreeCamera::BeforeFrame();
		HideUi::BeforeFrame();

		/* Misc */
		Config::BeforeFrame();
		FpsUnlocker::BeforeFrame();
		GlobalSpeedChanger::BeforeFrame();
		MenuSettings::BeforeFrame();
	}

	void OnFrame()
	{
		/* Adventure */
		DumbEnemies::OnFrame();
		Noclip::OnFrame();
		Peeking::OnFrame();

		/* Battle */
		BattleSpeedChanger::OnFrame();
		CritIndicator::OnFrame();

		/* Visuals */
		FovChanger::OnFrame();
		FpsIndicator::OnFrame();
		FreeCamera::OnFrame();
		HideUi::OnFrame();

		/* Misc */
		Config::OnFrame();
		FpsUnlocker::OnFrame();
		GlobalSpeedChanger::OnFrame();
		MenuSettings::OnFrame();
	}

	bool Setup()
	{
		HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");

		if (!hKernelBase)
		{
			return FALSE;
		}

		if (!IsHookActive(&LoadLibraryExWHook) && !CreateHook(&LoadLibraryExWHook, GetProcAddress(hKernelBase, "LoadLibraryExW"), LoadLibraryExWDetour, TRUE))
		{
			return FALSE;
		}

		return TRUE;
	}
}
