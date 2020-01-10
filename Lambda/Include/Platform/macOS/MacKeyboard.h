#pragma once
#include "LambdaCore.h"

#include "Core/Input/EKeyboard.h"

namespace Lambda
{
	//-----------
	//MacKeyboard
	//-----------

	class MacKeyboard
	{
	public:
		LAMBDA_STATIC_CLASS(MacKeyboard);

		static void Attach();
		
		static EKey   ConvertVirtualKey(uint32 keycode);
		static uint32 ConvertVirtualKey(EKey keycode);
		
		static uint32 GetModifierKeys(uint32 flags);
	private:
		static uint32 s_KeyTable[EKey::KEY_LAST + 1];
		static EKey	  s_MacKeyTable[256];
	};
}
