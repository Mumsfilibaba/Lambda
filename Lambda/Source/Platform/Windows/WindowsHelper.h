#pragma once
#include "LambdaCore.h"
#include "Core/Input/EKey.h"
#include "Core/Input/EMouseButton.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#if !defined(WIN32_LEAN_AND_MEAN)
		#define WIN32_LEAN_AND_MEAN 1
	#endif
	#include <Windows.h>

namespace Lambda
{
	//-----------------------
	//Windows HelperFunctions
	//-----------------------

	inline uint32 GetKeyModifers()
	{
		uint32 mods = 0;
		if (GetKeyState(VK_SHIFT) & 0x8000)
			mods |= KEY_MODIFIER_SHIFT;
		if (GetKeyState(VK_CONTROL) & 0x8000)
			mods |= KEY_MODIFIER_CONTROL;
		if (GetKeyState(VK_MENU) & 0x8000)
			mods |= KEY_MODIFIER_ALT;
		if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000)
			mods |= KEY_MODIFIER_SUPER;
		if (GetKeyState(VK_CAPITAL) & 1)
			mods |= KEY_MODIFIER_CAPS_LOCK;
		if (GetKeyState(VK_NUMLOCK) & 1)
			mods |= KEY_MODIFIER_NUM_LOCK;

		return mods;
	}


	EMouseButton WindowsButtonToLambda(uint32 keycode)
	{
		switch (keycode)
		{
		case MK_LBUTTON:	return MOUSEBUTTON_LEFT;
		case MK_MBUTTON:	return MOUSEBUTTON_MIDDLE;
		case MK_RBUTTON:	return MOUSEBUTTON_RIGHT;
		case MK_XBUTTON1:	return MOUSEBUTTON_FORWARD;
		case MK_XBUTTON2:	return MOUSEBUTTON_BACKWARD;
		default: return MOUSEBUTTON_UNKNOWN;
		}
	}


	uint32 LambdaButtonToWindows(EMouseButton keycode)
	{
		switch (keycode)
		{
		case MOUSEBUTTON_LEFT:		return MK_LBUTTON;
		case MOUSEBUTTON_MIDDLE:	return MK_MBUTTON;
		case MOUSEBUTTON_RIGHT:		return MK_RBUTTON;
		case MOUSEBUTTON_FORWARD:	return MK_XBUTTON1;
		case MOUSEBUTTON_BACKWARD:	return MK_XBUTTON2;
		default: return 0;
		}
	}
}
#endif