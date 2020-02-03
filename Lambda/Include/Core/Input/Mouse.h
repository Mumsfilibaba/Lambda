#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	enum EMouseButton : uint8
	{
		MOUSE_BUTTON_UNKNOWN	= 0,
		MOUSE_BUTTON_LEFT		= 1,
		MOUSE_BUTTON_MIDDLE		= 2,
		MOUSE_BUTTON_RIGHT		= 3,
		MOUSE_BUTTON_FORWARD	= 4,
		MOUSE_BUTTON_BACKWARD	= 5,
		MOUSE_BUTTON_LAST	= MOUSE_BUTTON_BACKWARD,
		MOUSE_BUTTON_COUNT	= MOUSE_BUTTON_LAST+1,
	};
}