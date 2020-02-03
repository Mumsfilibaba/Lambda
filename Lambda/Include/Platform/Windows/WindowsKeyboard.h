#pragma once
#include "LambdaCore.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Core/Input/Keyboard.h"

namespace Lambda
{
	//Not exported, meant to be an internal class
	class CWindowsKeyboard
	{
	public:
		LAMBDA_DECL_STATIC_CLASS(CWindowsKeyboard);

		static void Init();
		static uint32 GetActiveModifierKeys();

		_forceinline static uint32 ConvertVirtualKey(EKey keycode)
		{
			return s_KeyTable[keycode];
		}

		_forceinline static EKey ConvertVirtualKey(uint32 keycode)
		{
			return s_WindowsKeyTable[keycode];
		}
	private:
		static uint32 s_KeyTable[EKey::KEY_LAST + 1];
		static EKey	  s_WindowsKeyTable[256];
	};
}
#endif