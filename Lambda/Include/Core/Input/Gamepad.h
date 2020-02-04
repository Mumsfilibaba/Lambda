#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	enum EGamepad : uint32
	{
		GAMEPAD_PLAYER_UNKNOWN = 0,
		GAMEPAD_PLAYER_1 = 1,
		GAMEPAD_PLAYER_2 = 2,
		GAMEPAD_PLAYER_3 = 3,
		GAMEPAD_PLAYER_4 = 4,
		GAMEPAD_LAST	= GAMEPAD_PLAYER_4,
		GAMEPAD_COUNT	= GAMEPAD_LAST
	};

	enum EGamepadButton : uint8
	{
		GAMEPAD_BUTTON_UNKNOWN			= 0,
		GAMEPAD_BUTTON_PAD_UP			= 1,
		GAMEPAD_BUTTON_PAD_DOWN			= 2,
		GAMEPAD_BUTTON_PAD_LEFT			= 3,
		GAMEPAD_BUTTON_PAD_RIGHT		= 4,
		GAMEPAD_BUTTON_START			= 5,
		GAMEPAD_BUTTON_SELECT			= 6,
		GAMEPAD_BUTTON_LEFT_STICK		= 7,
		GAMEPAD_BUTTON_RIGHT_STICK		= 8,
		GAMEPAD_BUTTON_LEFT_TRIGGER		= 9,
		GAMEPAD_BUTTON_RIGHT_TRIGGER	= 10,
		GAMEPAD_BUTTON_FACE_UP			= 11,
		GAMEPAD_BUTTON_FACE_DOWN		= 12,
		GAMEPAD_BUTTON_FACE_LEFT		= 13,
		GAMEPAD_BUTTON_FACE_RIGHT		= 14,
		GAMEPAD_BUTTON_LAST		= GAMEPAD_BUTTON_FACE_RIGHT,
		GAMEPAD_BUTTON_COUNT	= GAMEPAD_BUTTON_LAST+1
	};
}