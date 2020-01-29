#pragma once
#include "LambdaCore.h"

#include "Core/Input/Mouse.h"
#include "Core/Input/Keyboard.h"
#include "Core/Input/MouseState.h"
#include "Core/Input/GamepadState.h
#include "Core/Input/KeyboardState.h"

namespace Lambda
{
    class CCommonInput
    {
    public:
        LAMBDA_DECL_STATIC_CLASS(CCommonInput);
        
        static void Init() {}
        static void Update() {}
        
        static bool IsKeyPressed(EKey)  { return false; }
        static bool IsKeyReleased(EKey) { return false; }
        static bool IsKeyModifierActive(EKeyModifier) { return false; }
        
        static bool IsMouseButtonPressed(EMouseButton)  { return false; }
        static bool IsMouseButtonReleased(EMouseButton) { return false; }
        static void SetMousePosition(int32 x, int32 y) {}
        static void GetMousePosition(int32& x, int32& y) {}
        
        static CMouseState GetMouseState() { return CMouseState(); }
        static CGamepadState GetGamepadState() { return CGamepadState(); }
        static CKeyboardState GetKeyboardState() { return CKeyboardState(); }
    };
}
