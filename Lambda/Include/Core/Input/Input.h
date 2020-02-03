#pragma once
#include "LambdaCore.h"
#include "Mouse.h"
#include "Gamepad.h"
#include "Keyboard.h"

namespace Lambda
{
    struct SMouseState;
    struct SGamepadState;
    struct SKeyboardState;
    
    class LAMBDA_API CInput final
    {
    public:
        LAMBDA_DECL_STATIC_CLASS(CInput);

        static bool IsKeyPressed(EKey key);
        static bool IsKeyReleased(EKey key);
        static bool IsModiferKeyActive(EKeyModifier modiferKey);
        static uint32 GetModiferKeys();

        static bool IsMouseButtonPressed(EMouseButton button);
        static bool IsMouseButtonReleased(EMouseButton button);

        static void SetMousePosition(int32 x, int32 y);
        static void GetMousePosition(int32& x, int32& y);

        static void GetMouseState(SMouseState& mouseState);
        static void GetGamepadState(SGamepadState& gamepadState, EGamepad gamepad);
        static void GetKeyboardState(SKeyboardState& keyboardState);
    };
}
