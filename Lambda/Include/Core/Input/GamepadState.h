#pragma once
#include "Gamepad.h"

namespace Lambda
{
	struct SGamepadState
	{
	private:
		struct SGamepadStick
		{
			float x;
			float y;
		};

	public:
		_forceinline SGamepadState()
		{
			memset(this, 0, sizeof(SGamepadState));

			Player = GAMEPAD_PLAYER_UNKNOWN;
			bIsConnected = false;
		}

		_forceinline bool IsButtonPressed(EGamepadButton button) const
		{
			return bButtonStates[button] == true;
		}

		_forceinline bool IsButtonReleased(EGamepadButton button) const
		{
			return bButtonStates[button] == false;
		}

	public:
		EGamepad Player = EGamepad::GAMEPAD_PLAYER_UNKNOWN;
		SGamepadStick LeftStick;
		SGamepadStick RightStick;
		float LeftTrigger;
		float RightTrigger;
		bool bButtonStates[EGamepadButton::GAMEPAD_BUTTON_COUNT];
		bool bIsConnected;
	};
}