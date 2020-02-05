#pragma once
#include "LambdaCore.h"

#ifdef LAMBDA_PLAT_MACOS

namespace Lambda
{
    class CMacInput final : public CommonInput
    {
    public:
        LAMBDA_DECL_STATIC_CLASS(CMacInput);
        
        static void Init();
        static void Update();
        
        static bool IsKeyPressed(EKey key);
        static bool IsKeyReleased(EKey key);
        static bool IsKeyModifierActive(EKeyModifier modifier);
        
        static bool IsMouseButtonPressed(EMouseButton button);
        static bool IsMouseButtonReleased(EMouseButton button);
        static void SetMousePosition(int32 x, int32 y);
        static void GetMousePosition(int32& x, int32& y);
        
        static CMouseState GetMouseState();
        static CGamepadState GetGamepadState();
        static CKeyboardState GetKeyboardState();
    };

    typedef CMacInput Input;
}
#endif
