#pragma once
#include "LambdaCore.h"

#ifdef LAMBDA_PLAT_MACOS
    #include "Platform/Common/CommonInput.h"

namespace Lambda
{
    class IWindow;

    class MacInput final : public CommonInput
    {
    public:
        LAMBDA_DECL_STATIC_CLASS(MacInput);
        
        static void Init();
        static void Update();
        
        static void SetMousePosition(IWindow* pRelativeWindow, int32 x, int32 y);
        static void GetMousePosition(IWindow* pRelativeWindow, int32& x, int32& y);
        
        static uint32 GetModiferKeys();

        _forceinline static bool IsKeyPressed(EKey key)
        {
            return s_KeyboardState.IsKeyPressed(key);
        }
        
        _forceinline static bool IsKeyReleased(EKey key)
        {
            return s_KeyboardState.IsKeyReleased(key);
        }
        
        _forceinline static bool IsMouseButtonPressed(EMouseButton button)
        {
            return s_MouseState.IsButtonPressed(button);
        }
        
        _forceinline static bool IsMouseButtonReleased(EMouseButton button)
        {
            return s_MouseState.IsButtonReleased(button);
        }
        
        _forceinline static SMouseState GetMouseState()
        {
            return s_MouseState;
        }
        
        _forceinline static SGamepadState GetGamepadState(EGamepad)
        {
            //TODO: Fix real gamepad states for macOS
            return SGamepadState();
        }
        
        _forceinline static SKeyboardState GetKeyboardState()
        {
            return s_KeyboardState;
        }
        
    private:
        static SMouseState s_MouseState;
        static SKeyboardState s_KeyboardState;
    };

    typedef MacInput Input;
}
#endif
