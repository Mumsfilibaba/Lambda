#include "LambdaPch.h"

#include "Core/Engine.h"
#include "Core/Input/Input.h"

#include "Platform/PlatformInput.h"

namespace Lambda
{
    bool CInput::IsKeyPressed(EKey key)
    {
        return PlatformInput::IsKeyPressed(key);
    }

    bool CInput::IsKeyReleased(EKey key)
    {
        return PlatformInput::IsKeyReleased(key);
    }

    bool CInput::IsModiferKeyActive(EKeyModifier modiferKey)
    {
        uint32 modifers = GetModiferKeyMask();
        return modifers & modiferKey;
    }

    uint32 CInput::GetModiferKeyMask()
    {
        return PlatformInput::GetModiferKeys();
    }
        
    bool CInput::IsMouseButtonPressed(EMouseButton button)
    {
        return PlatformInput::IsMouseButtonPressed(button);
    }

    bool CInput::IsMouseButtonReleased(EMouseButton button)
    {
        return PlatformInput::IsMouseButtonReleased(button);
    }

    void CInput::SetMousePosition(int32 x, int32 y)
    {   
        IWindow* pWindow = g_Engine.GetWindow();
        PlatformInput::SetMousePosition(pWindow, x, y);
    }

    void CInput::GetMousePosition(int32& x, int32& y)
    {   
        IWindow* pWindow = g_Engine.GetWindow();
        PlatformInput::GetMousePosition(pWindow, x, y);
    }
        
    void CInput::GetMouseState(SMouseState& mouseState)
    {
        mouseState = PlatformInput::GetMouseState();
    }

    void CInput::GetGamepadState(SGamepadState& gamepadState, EGamepad gamepad)
    {
        gamepadState = PlatformInput::GetGamepadState(gamepad);
    }
        
    void CInput::GetKeyboardState(SKeyboardState& keyboardState)
    {
        keyboardState = PlatformInput::GetKeyboardState();
    }
}
