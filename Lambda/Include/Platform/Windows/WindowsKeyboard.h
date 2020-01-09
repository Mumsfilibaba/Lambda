#pragma once
#include "LambdaCore.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Core/Input/EKeyboard.h"

namespace Lambda
{
	//---------------
	//WindowsKeyboard
	//---------------
	class WindowsKeyboard
	{
	public:
		LAMBDA_STATIC_CLASS(WindowsKeyboard);

		static void Initialize();
		
		static uint32 ConvertVirtualKey(EKey keycode);
		static EKey   ConvertVirtualKey(uint32 keycode);

		static uint32 GetModifierKeys();
	private:
		static uint32 s_KeyTable[EKey::KEY_LAST+1];
		static EKey	  s_WindowsKeyTable[256];
	};
}
#endif