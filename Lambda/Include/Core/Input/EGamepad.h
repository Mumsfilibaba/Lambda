#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    //--------
    //EGamepad
    //--------
    enum EGamepad : uint8
    {
        GAMEPAD_UNKNOWN = 0,
        GAMEPAD_1       = 1,
        GAMEPAD_2       = 2,
        GAMEPAD_3       = 3,
        GAMEPAD_4       = 4,
        GAMEPAD_LAST    = GAMEPAD_4
    };

	//-------------
	//EGamepadStick
	//-------------
	enum EGamepadStick : uint8
	{
		GAMEPAD_STICK_UNKNOWN = 0,
		GAMEPAD_STICK_LEFT	  = 1,
		GAMEPAD_STICK_RIGHT	  = 2,
		GAMEPAD_STICK_LAST	  = GAMEPAD_STICK_RIGHT,
	};

	//---------------
	//EGamepadTrigger
	//---------------
	enum EGamepadTrigger : uint8
	{
		GAMEPAD_TRIGGER_UNKNOWN = 0,
		GAMEPAD_TRIGGER_LEFT	= 1,
		GAMEPAD_TRIGGER_RIGHT	= 2,
		GAMEPAD_TRIGGER_LAST    = GAMEPAD_TRIGGER_RIGHT,
	};
}
