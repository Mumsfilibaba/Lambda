#pragma once
#include "LambdaCore.h"

#ifdef LAMBDA_PLAT_MACOS
    #include "Core/Input/Keyboard.h"

namespace Lambda
{
    class CMacKeyboard
    {
    public:
        LAMBDA_DECL_STATIC_CLASS(CMacKeyboard);

        static void Init();
        
        static EKey ConvertVirtualKey(uint32 keycode);
        static uint32 ConvertVirtualKey(EKey keycode);
        
        static uint32 GetModifierKeys(uint32 flags);
    private:
        static uint32 s_KeyTable[EKey::KEY_LAST + 1];
        static EKey s_MacKeyTable[256];
    };
}
#endif
