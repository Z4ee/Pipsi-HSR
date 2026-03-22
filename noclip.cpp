#include "pch.h"

#include "noclip.h"

namespace Noclip
{
	static void OnUpdate(UnityEngine::Transform* PlayerTransform)
	{
		UnityEngine::Vector3 Position = PlayerTransform->get_position();
		UnityEngine::Vector3 Direction = {};

		if (Options.NoclipUseCameraDirection)
		{
			UnityEngine::Camera* Camera = UnityEngine::Camera::get_main();

			if (Camera)
			{
				UnityEngine::Transform* CameraTransform = Camera->get_transform();

				if (CameraTransform)
				{
					if (Input::IsKeyHeld('W'))
					{
						Direction += CameraTransform->get_forward();
					}

					if (Input::IsKeyHeld('S'))
					{
						Direction -= CameraTransform->get_forward();
					}

					if (Input::IsKeyHeld('A'))
					{
						Direction -= CameraTransform->get_right();
					}

					if (Input::IsKeyHeld('D'))
					{
						Direction += CameraTransform->get_right();
					}
				}
			}
		}
		else
		{
			if (Input::IsKeyHeld('W') ||
				Input::IsKeyHeld('S') ||
				Input::IsKeyHeld('A') ||
				Input::IsKeyHeld('D'))
			{
				Direction += PlayerTransform->get_forward();
			}
		}

		if (Input::IsKeyHeld(VK_SPACE))
		{
			Direction += PlayerTransform->get_up();
		}

		if (Input::IsKeyHeld(VK_CONTROL))
		{
			Direction -= PlayerTransform->get_up();
		}

		if (Direction.x != 0.0f || Direction.y != 0.0f || Direction.z != 0.0f)
		{
			float Speed = Options.NoclipSpeed;

			if (Input::IsKeyHeld(VK_SHIFT))
			{
				Speed *= 3.0f;
			}

			PlayerTransform->set_position(Direction * Speed * UnityEngine::Time::get_deltaTime());
		}
	}

	static bool IsAdventurePhaseAndPlayerCanMove()
	{
		RPG::Client::GamePhaseManager** GamePhaseManager = RPG::Client::GlobalVars::StaticGet_s_GamePhaseManager();

		if (!GamePhaseManager || !*GamePhaseManager)
		{
			return FALSE;
		}

		if ((*GamePhaseManager)->_CurrentGamePhaseType_k__BackingField != RPG::Client::GamePhaseType::Adventure)
		{
			return FALSE;
		}

		RPG::Client::ModuleManager** ModuleManager = RPG::Client::GlobalVars::StaticGet_s_ModuleManager();

		if (!ModuleManager || !*ModuleManager)
		{
			return FALSE;
		}

		RPG::Client::GamePlayLockModule* GamePlayLockModule = (*ModuleManager)->GamePlayLockModule;

		if (!GamePlayLockModule)
		{
			return FALSE;
		}

		if (GamePlayLockModule->IsLockBy(RPG::Client::LockSource::TaskLockPlayerControl))
		{
			return FALSE;
		}

		return TRUE;
	}

	static HOOK SetPositionAndRotationHook = {};

	static System::Void SetPositionAndRotationDetour(UnityEngine::Transform* _this, UnityEngine::Vector3 position, UnityEngine::Quaternion rotation)
	{
		if (Options.Noclip && Input::GetKeyStateValue(Options.NoclipKey, Options.NoclipKeyHeld))
		{
			RPG::GameCore::GameEntity* LocalPlayer = RPG::GameCore::AdventureStatic::GetLocalPlayer();

			if (LocalPlayer)
			{
				UnityEngine::GameObject* UnityGO = LocalPlayer->get_UnityGO();

				if (UnityGO)
				{
					if (_this == UnityGO->get_transform())
					{
						if (IsAdventurePhaseAndPlayerCanMove())
						{
							return;
						}
					}
				}
			}
		}

		return CALL_ORIGINAL(SetPositionAndRotationHook, SetPositionAndRotationDetour, _this, position, rotation);
	}

	static HOOK TickHook = {};

	static System::Void TickDetour(RPG::GameCore::GameEntity* _this, System::Single fElapsedTimeInSec)
	{
		if (Options.Noclip && Input::GetKeyStateValue(Options.NoclipKey, Options.NoclipKeyHeld))
		{
			if (_this == RPG::GameCore::AdventureStatic::GetLocalPlayer())
			{
				UnityEngine::GameObject* UnityGO = _this->get_UnityGO();

				if (UnityGO)
				{
					UnityEngine::Transform* Transform = UnityGO->get_transform();

					if (Transform)
					{
						if (IsAdventurePhaseAndPlayerCanMove())
						{
							OnUpdate(Transform);
						}
					}
				}
			}
		}

		return CALL_ORIGINAL(TickHook, TickDetour, _this, fElapsedTimeInSec);
	}

	void Menu()
	{
		ImGui::BeginGroupPanel("Noclip");

		ImGui::Checkbox("Enable", &Options.Noclip);

		if (Options.Noclip)
		{
			ImGui::Indent();

			ImGui::SliderFloat("Speed", &Options.NoclipSpeed, 0.f, 100.f, "%.1f");
			ImGui::Checkbox("Use Camera Direction", &Options.NoclipUseCameraDirection);

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
		if (!IsHookActive(&SetPositionAndRotationHook) && !CreateHook(&SetPositionAndRotationHook, (PBYTE)hIl2Cpp + UNITYENGINE_TRANSFORM_SETPOSITIONANDROTATION_OFFSET, SetPositionAndRotationDetour, TRUE))
		{
			return FALSE;
		}

		if (!IsHookActive(&TickHook) && !CreateHook(&TickHook, (PBYTE)hIl2Cpp + RPG_GAMECORE_GAMEENTITY_TICK_OFFSET, TickDetour, TRUE))
		{
			return FALSE;
		}

		return TRUE;
	}
}
