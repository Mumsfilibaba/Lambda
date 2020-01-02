#pragma once
#include "LambdaCore.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Core/Input/EKey.h"

	#define WIN32_LEAN_AND_MEAN 1
	#include <Windows.h>

namespace Lambda
{
	namespace WindowsUtilities
	{
		inline uint32 GetModifierKeys()
		{
			uint32 mods = 0;
			if (GetKeyState(VK_SHIFT) & 0x8000)
				mods |= EKeyModifier::KEY_MODIFIER_SHIFT;
			if (GetKeyState(VK_CONTROL) & 0x8000)
				mods |= EKeyModifier::KEY_MODIFIER_CONTROL;
			if (GetKeyState(VK_MENU) & 0x8000)
				mods |= EKeyModifier::KEY_MODIFIER_ALT;
			if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000)
				mods |= EKeyModifier::KEY_MODIFIER_SUPER;
			if (GetKeyState(VK_CAPITAL) & 1)
				mods |= EKeyModifier::KEY_MODIFIER_CAPS_LOCK;
			if (GetKeyState(VK_NUMLOCK) & 1)
				mods |= EKeyModifier::KEY_MODIFIER_NUM_LOCK;

			return mods;
		}
	}
}
#endif