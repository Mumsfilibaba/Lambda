#pragma once
#include "LambdaCore.h"

#include "Core/Input/Mouse.h"
#include "Core/Input/Keyboard.h"
#include "Core/Input/MouseState.h"
#include "Core/Input/GamepadState.h"
#include "Core/Input/KeyboardState.h"

namespace Lambda
{
    class IWindow;

    class CommonInput
    {
    public:
        LAMBDA_DECL_STATIC_CLASS(CommonInput);
        
        static void Init() {}
        static void Update() {}
        
        static bool IsKeyPressed(EKey)  { return false; }
        static bool IsKeyReleased(EKey) { return false; }
        static uint32 GetModiferKeys()  { return 0; }
        
        static bool IsMouseButtonPressed(EMouseButton)  { return false; }
        static bool IsMouseButtonReleased(EMouseButton) { return false; }
        static void SetMousePosition(IWindow*, int32, int32) {}
        static void GetMousePosition(IWindow*, int32&, int32&) {}
        
        static SMouseState GetMouseState() { return SMouseState(); }
        static SGamepadState GetGamepadState(EGamepad) { return SGamepadState(); }
        static SKeyboardState GetKeyboardState() { return SKeyboardState(); }
    };
}
