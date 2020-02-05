#include "LambdaPch.h"

#include "Core/Engine.h"
#include "Core/Input/Input.h"

#include "Platform/PlatformInput.h"

namespace Lambda
{
    bool Input::IsKeyPressed(EKey key)
    {
        return PlatformInput::IsKeyPressed(key);
    }

    bool Input::IsKeyReleased(EKey key)
    {
        return PlatformInput::IsKeyReleased(key);
    }

    bool Input::IsModiferKeyActive(EKeyModifier modiferKey)
    {
        uint32 modifers = GetModiferKeyMask();
        return modifers & modiferKey;
    }

    uint32 Input::GetModiferKeyMask()
    {
        return PlatformInput::GetModiferKeys();
    }
        
    bool Input::IsMouseButtonPressed(EMouseButton button)
    {
        return PlatformInput::IsMouseButtonPressed(button);
    }

    bool Input::IsMouseButtonReleased(EMouseButton button)
    {
        return PlatformInput::IsMouseButtonReleased(button);
    }

    void Input::SetMousePosition(int32 x, int32 y)
    {   
        IWindow* pWindow = g_Engine.GetWindow();
        PlatformInput::SetMousePosition(pWindow, x, y);
    }

    void Input::GetMousePosition(int32& x, int32& y)
    {   
        IWindow* pWindow = g_Engine.GetWindow();
        PlatformInput::GetMousePosition(pWindow, x, y);
    }
        
    void Input::GetMouseState(SMouseState& mouseState)
    {
        mouseState = PlatformInput::GetMouseState();
    }

    void Input::GetGamepadState(SGamepadState& gamepadState, EGamepad gamepad)
    {
        gamepadState = PlatformInput::GetGamepadState(gamepad);
    }
        
    void Input::GetKeyboardState(SKeyboardState& keyboardState)
    {
        keyboardState = PlatformInput::GetKeyboardState();
    }
}
