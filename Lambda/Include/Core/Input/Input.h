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

    namespace Input
    {
        LAMBDA_API bool IsKeyPressed(EKey key);
        LAMBDA_API bool IsKeyReleased(EKey key);
        LAMBDA_API bool IsModiferKeyActive(EKeyModifier modiferKey);
        LAMBDA_API uint32 GetModiferKeys();
        
        LAMBDA_API bool IsMouseButtonPressed(EMouseButton button);
        LAMBDA_API bool IsMouseButtonReleased(EMouseButton button);
    
        LAMBDA_API void SetMousePosition(int32 x, int32 y);
        LAMBDA_API void GetMousePosition(int32& x, int32& y);

        LAMBDA_API void GetMouseState(SMouseState& mouseState);
        LAMBDA_API void GetGamepadState(SGamepadState& gamepadState, EGamepad gamepad);
        LAMBDA_API void GetKeyboardState(SKeyboardState& keyboardState);
    }
}
