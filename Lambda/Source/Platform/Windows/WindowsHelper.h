#pragma once
#include "Defines.h"
#include "Types.h"
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

	static uint32 GetKeyModifers()
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
}
#endif