#pragma once
#include "LambdaCore.h"

#ifdef LAMBDA_PLAT_MACOS
    #include "Core/Input/Keyboard.h"

namespace Lambda
{
    class MacKeyboard
    {
    public:
        LAMBDA_DECL_STATIC_CLASS(MacKeyboard);

        static void Init();
        static uint32 GetModifierKeys(uint32 flags);
        
        _forceinline EKey ConvertVirtualKey(uint32 keycode)
        {
            return s_MacKeyTable[keycode];
        }
        
        _forceinline uint32 ConvertVirtualKey(EKey keycode)
        {
            return s_KeyTable[keycode];
        }
        
    private:
        static uint32 s_KeyTable[EKey::KEY_LAST + 1];
        static EKey s_MacKeyTable[256];
    };
}
#endif
