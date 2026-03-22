#include "pch.h"

#include "free_camera.h"

namespace FreeCamera
{
	static void OnCameraSetPosition(UnityEngine::Transform* _this, UnityEngine::Vector3* value)
	{
		float MoveX = 0.0f;
		float MoveZ = 0.0f;

		if (Input::IsKeyHeld(Options.FreeCameraForwardKey))
		{
			MoveZ = 1.0f;
		}

		if (Input::IsKeyHeld(Options.FreeCameraBackwardKey))
		{
			MoveZ = -1.0f;
		}

		if (Input::IsKeyHeld(Options.FreeCameraRightKey))
		{
			MoveX = 1.0f;
		}

		if (Input::IsKeyHeld(Options.FreeCameraLeftKey))
		{
			MoveX = -1.0f;
		}

		float Speed = Options.FreeCameraSpeed;

		if (Input::IsKeyHeld(Options.FreeCameraBoostKey))
		{
			Speed *= 3;
		}

		UnityEngine::Vector3 Position = _this->get_position();
		UnityEngine::Vector3 Direction = _this->TransformDirection_1(MoveX, 0.0f, MoveZ);

		if (Input::IsKeyHeld(Options.FreeCameraUpKey))
		{
			Direction.y = 1.0f;
		}

		if (Input::IsKeyHeld(Options.FreeCameraDownKey))
		{
			Direction.y = -1.0f;
		}

		*value = Position + Direction * Speed * UnityEngine::Time::get_unscaledDeltaTime();
	}

	static float RotationX = 0.0f;
	static float RotationY = 0.0f;
	static bool RotationInitialized = FALSE;

	static void OnCameraSetRotation(UnityEngine::Transform* _this, UnityEngine::Quaternion* value)
	{
		if (!RotationInitialized)
		{
			UnityEngine::Vector3 EulerAngles = _this->get_eulerAngles();

			RotationX = EulerAngles.y;
			RotationY = EulerAngles.x;

			RotationInitialized = TRUE;
		}

		float MouseX = UnityEngine::Input::GetAxis(SYSTEM_STRING("Mouse X")) * Options.FreeCameraMouseSensitivity;
		float MouseY = UnityEngine::Input::GetAxis(SYSTEM_STRING("Mouse Y")) * Options.FreeCameraMouseSensitivity;

		RotationX += MouseX;
		RotationY -= MouseY;

		float MaxVerticalAngle = 90.0f;

		if (RotationY < -MaxVerticalAngle)
		{
			RotationY = -MaxVerticalAngle;
		}

		if (RotationY > MaxVerticalAngle)
		{
			RotationY = MaxVerticalAngle;
		}

		*value = UnityEngine::Quaternion::Euler(RotationY, RotationX, 0.0f);
	}

	static HOOK set_position_InjectedHook = {};

	static System::Void set_position_InjectedDetour(UnityEngine::Transform* _this, UnityEngine::Vector3* value)
	{
		if (Options.FreeCamera && Input::GetKeyStateValue(Options.FreeCameraKey, Options.FreeCameraKeyHeld))
		{
			UnityEngine::Camera* Camera = UnityEngine::Camera::get_main();

			if (Camera)
			{
				if (_this == Camera->get_transform())
				{
					OnCameraSetPosition(_this, value);
				}
			}
		}

		return CALL_ORIGINAL(set_position_InjectedHook, set_position_InjectedDetour, _this, value);
	}

	static HOOK set_rotation_InjectedHook = {};

	static System::Void set_rotation_InjectedDetour(UnityEngine::Transform* _this, UnityEngine::Quaternion* value)
	{
		if (Options.FreeCamera && Input::GetKeyStateValue(Options.FreeCameraKey, Options.FreeCameraKeyHeld))
		{
			UnityEngine::Camera* Camera = UnityEngine::Camera::get_main();

			if (Camera)
			{
				if (_this == Camera->get_transform())
				{
					OnCameraSetRotation(_this, value);
				}
			}
		}
		else
		{
			RotationInitialized = FALSE;
		}

		return CALL_ORIGINAL(set_rotation_InjectedHook, set_rotation_InjectedDetour, _this, value);
	}

	void Menu()
	{
		ImGui::BeginGroupPanel("Free Camera");

		ImGui::Checkbox("Enable", &Options.FreeCamera);
		ImGui::SameLine();
		ImGui::Hotkey("##FreeCameraHotkey", &Options.FreeCameraKey, &Options.FreeCameraKeyHeld);

		if (Options.FreeCamera)
		{
			ImGui::Indent();

			ImGui::SliderFloat("Speed", &Options.FreeCameraSpeed, 0.f, 100.f, "%.1f");
			ImGui::SliderFloat("Mouse Sensitivity", &Options.FreeCameraMouseSensitivity, 0.f, 10.f, "%.2f");

			ImGui::Text("Move Forward:");
			ImGui::SameLine();
			ImGui::SmallHotkey("##KeyForward", &Options.FreeCameraForwardKey);

			ImGui::Text("Move Backward:");
			ImGui::SameLine();
			ImGui::SmallHotkey("##KeyBackward", &Options.FreeCameraBackwardKey);

			ImGui::Text("Move Right:");
			ImGui::SameLine();
			ImGui::SmallHotkey("##KeyRight", &Options.FreeCameraRightKey);

			ImGui::Text("Move Left:");
			ImGui::SameLine();
			ImGui::SmallHotkey("##KeyLeft", &Options.FreeCameraLeftKey);

			ImGui::Text("Move Up:");
			ImGui::SameLine();
			ImGui::SmallHotkey("##KeyUp", &Options.FreeCameraUpKey);

			ImGui::Text("Move Down:");
			ImGui::SameLine();
			ImGui::SmallHotkey("##KeyDown", &Options.FreeCameraDownKey);

			ImGui::Text("Boost:");
			ImGui::SameLine();
			ImGui::SmallHotkey("##KeyBoost", &Options.FreeCameraBoostKey);

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
		if (!IsHookActive(&set_position_InjectedHook) && !CreateHook(&set_position_InjectedHook, (PBYTE)hIl2Cpp + UNITYENGINE_TRANSFORM_SET_POSITION_INJECTED_OFFSET, set_position_InjectedDetour, TRUE))
		{
			return FALSE;
		}

		if (!IsHookActive(&set_rotation_InjectedHook) && !CreateHook(&set_rotation_InjectedHook, (PBYTE)hIl2Cpp + UNITYENGINE_TRANSFORM_SET_ROTATION_INJECTED_OFFSET, set_rotation_InjectedDetour, TRUE))
		{
			return FALSE;
		}

		return TRUE;
	}
}
