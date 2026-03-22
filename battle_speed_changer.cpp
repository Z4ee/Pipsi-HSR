#include "pch.h"

#include "battle_speed_changer.h"

namespace BattleSpeedChanger
{
	static PVOID LastShit = NULL;
	static float LastSpeedUpRatio = 1.0f;
	static int LastTotalSkillPerformTime = 0;
	static int LastRealTimeCounter = 0;

	static HOOK get_s_SimulationSpeedHook = {};

	static System::Single get_s_SimulationSpeedDetour()
	{
		if (Options.BattleSpeedChanger && Input::GetKeyStateValue(Options.BattleSpeedChangerKey, Options.BattleSpeedChangerKeyHeld))
		{
			RPG::Client::GamePhaseManager* GamePhaseManager = *RPG::Client::GlobalVars::StaticGet_s_GamePhaseManager();

			if (GamePhaseManager)
			{
				RPG::Client::GamePhaseType GamePhaseType = GamePhaseManager->get_CurrentGamePhaseType();

				if (GamePhaseType == RPG::Client::GamePhaseType::Battle)
				{
					RPG::Client::BattleGamePhase* BattleGamePhase = (RPG::Client::BattleGamePhase*)GamePhaseManager->_CurrentPhase;

					if (BattleGamePhase)
					{
						RPG::GameCore::BattleInstance* BattleInstance = BattleGamePhase->_BattleInstance;

						if (BattleInstance)
						{
							RPG::GameCore::GameWorld* GameWorld = BattleInstance->_GameWorld;

							if (GameWorld && !GameWorld->get_IsPause())
							{
								return Options.BattleSpeedChangerValue;
							}
						}
					}
				}
			}
		}

		return CALL_ORIGINAL(get_s_SimulationSpeedHook, get_s_SimulationSpeedDetour);
	}

	static HOOK TickHook = {};

	static System::Void TickDetour(Class_2_C53551EA56D92DA8* _this, System::Single a1)
	{
		RPG::Client::GamePhaseManager* GamePhaseManager = *RPG::Client::GlobalVars::StaticGet_s_GamePhaseManager();

		if (GamePhaseManager)
		{
			RPG::Client::GamePhaseType GamePhaseType = GamePhaseManager->_CurrentGamePhaseType_k__BackingField;

			if (GamePhaseType == RPG::Client::GamePhaseType::Battle)
			{
				RPG::Client::BattleGamePhase* BattleGamePhase = (RPG::Client::BattleGamePhase*)GamePhaseManager->_CurrentPhase;

				if (BattleGamePhase)
				{
					RPG::GameCore::BattleInstance* BattleInstance = BattleGamePhase->_BattleInstance;

					if (BattleInstance)
					{
						RPG::GameCore::TurnBasedGameMode* TurnBasedGameMode = BattleInstance->_TurnBasedGameMode;
						RPG::GameCore::GameWorld* GameWorld = BattleInstance->_GameWorld;

						if (TurnBasedGameMode && GameWorld)
						{
							RPG::GameCore::EntityManager* EntityManager = GameWorld->_EntityManager;

							if (EntityManager)
							{
								RPG::GameCore::GameEntity* LevelEntity = EntityManager->_LevelEntity_k__BackingField;

								if (LevelEntity)
								{
									RPG::GameCore::StatisticsComponent* StatisticsComponent = GetComponent<RPG::GameCore::StatisticsComponent>(LevelEntity, RPG::GameCore::StatisticsComponent_TypeDefinitionIndex);

									if (StatisticsComponent)
									{
										if (StatisticsComponent->_totalBattleTurns == 0)
										{
											LastSpeedUpRatio = RPG::Client::BattleUIUtils::GetSpeedUpRatio();
										}

										LastTotalSkillPerformTime = (int)(StatisticsComponent->_totalSkillPerformTime / LastSpeedUpRatio);
										LastRealTimeCounter = (int)TurnBasedGameMode->get_RealTimeCounter();

										if (GameWorld->get_IsPause())
										{
											LastShit = _this; // just not null
										}
										else
										{
											LastShit = _this->Field_2_4;
										}

										if (LastTotalSkillPerformTime > LastRealTimeCounter)
										{
											return;
										}
									}
								}
							}
						}
					}
				}
			}
		}

		CALL_ORIGINAL(TickHook, TickDetour, _this, a1);
	}

	static HOOK DisposeHook = {};

	static System::Void DisposeDetour(Class_2_C53551EA56D92DA8* _this)
	{
		LastShit = NULL;
		LastTotalSkillPerformTime = 0;
		LastRealTimeCounter = 0;

		CALL_ORIGINAL(DisposeHook, DisposeDetour, _this);
	}

	static HOOK TryQuitBattleHook = {};

	static System::Void TryQuitBattleDetour(RPG::GameCore::TurnBasedGameMode* _this, System::Boolean bFromReplay, System::Boolean fromServerNotify, System::Boolean manualExitRetry)
	{
		if (LastTotalSkillPerformTime > LastRealTimeCounter)
		{
			return;
		}

		CALL_ORIGINAL(TryQuitBattleHook, TryQuitBattleDetour, _this, bFromReplay, fromServerNotify, manualExitRetry);
	}

	void Menu()
	{
		ImGui::BeginGroupPanel("Battle Speed Changer");

		ImGui::Checkbox("Enable", &Options.BattleSpeedChanger);
		ImGui::SameLine();
		ImGui::Hotkey("##BattleSpeedHotkey", &Options.BattleSpeedChangerKey, &Options.BattleSpeedChangerKeyHeld);

		if (Options.BattleSpeedChanger)
		{
			ImGui::Indent();

			ImGui::SliderFloat("Speed", &Options.BattleSpeedChangerValue, 0.1f, 100.0f, "%.1f");

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
		if (!IsHookActive(&get_s_SimulationSpeedHook) && !CreateHook(&get_s_SimulationSpeedHook, (PBYTE)hIl2Cpp + RPG_CLIENT_GLOBALVARS_GET_S_SIMULATIONSPEED_OFFSET, get_s_SimulationSpeedDetour, FALSE))
		{
			return FALSE;
		}

		if (!IsHookActive(&TickHook) && !CreateHook(&TickHook, (PBYTE)hIl2Cpp + CLASS_2_C53551EA56D92DA8_TICK_OFFSET, TickDetour, FALSE))
		{
			return FALSE;
		}

		if (!IsHookActive(&DisposeHook) && !CreateHook(&DisposeHook, (PBYTE)hIl2Cpp + CLASS_2_C53551EA56D92DA8_DISPOSE_OFFSET, DisposeDetour, FALSE))
		{
			return FALSE;
		}

		if (!IsHookActive(&TryQuitBattleHook) && !CreateHook(&TryQuitBattleHook, (PBYTE)hIl2Cpp + RPG_GAMECORE_TURNBASEDGAMEMODE_TRYQUITBATTLE_OFFSET, TryQuitBattleDetour, FALSE))
		{
			return FALSE;
		}

		return TRUE;
	}
}
