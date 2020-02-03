#pragma once
#include "Mouse.h"

namespace Lambda
{
	struct SMouseState
	{
	public:
		_forceinline SMouseState() 
		{
			memset(this, 0, sizeof(SMouseState));
		}

		_forceinline bool IsButtonPressed(EMouseButton button) const
		{
			return bButtonStates[button] == true;
		}

		_forceinline bool IsButtonReleased(EMouseButton button) const
		{
			return bButtonStates[button] == false;
		}
	public:
		struct
		{
			int32 x;
			int32 y;
		} Position;
		struct
		{
			float Vertical;
			float Horizontal;
		} ScrollWheel;
		bool bButtonStates[EMouseButton::MOUSE_BUTTON_COUNT];
	};
}
