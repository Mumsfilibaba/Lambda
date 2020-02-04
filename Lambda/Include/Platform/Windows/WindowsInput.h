#pragma once
#include "LambdaCore.h"

#ifdef LAMBDA_PLAT_WINDOWS
#include "Windows.h"
#include "WindowsKeyboard.h"

#include "Platform/Common/CommonInput.h"

namespace Lambda
{
	class LAMBDA_API CWindowsInput : public CCommonInput
	{
	public:
		LAMBDA_DECL_STATIC_CLASS(CWindowsInput);

        static void Init();
        static void Update();

        static void SetMousePosition(IWindow* pRelativeWindow, int32 x, int32 y);
        static void GetMousePosition(IWindow* pRelativeWindow, int32& x, int32& y);

        static void OnKeyboardMessage(uint32 virtualKey, bool bKeyDown);
        static void OnMouseButtonMessage(uint32 message, WPARAM wParam);

        _forceinline static bool IsKeyPressed(EKey key)
        { 
            return s_KeyboardState.IsKeyPressed(key);
        }

        _forceinline static bool IsKeyReleased(EKey key)
        { 
            return s_KeyboardState.IsKeyReleased(key);
        }

        _forceinline static uint32 GetModiferKeys() 
        { 
            return CWindowsKeyboard::GetActiveModifierKeys();
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

        _forceinline static SGamepadState GetGamepadState(EGamepad gamepad) 
        { 
            const uint32 index = gamepad - 1; 
            LAMBDA_ASSERT_MSG(index < EGamepad::GAMEPAD_COUNT, "Invalid Gamepad");
            return s_GamepadStates[index];
        }

        _forceinline static SKeyboardState GetKeyboardState()
        {
            return s_KeyboardState;
        }
    private:
        static SMouseState s_MouseState;
        static SKeyboardState s_KeyboardState;
        static SGamepadState s_GamepadStates[EGamepad::GAMEPAD_COUNT];
        static DWORD s_GamepadPacketIDs[EGamepad::GAMEPAD_COUNT];
	};

    typedef CWindowsInput PlatformInput;
}
#endif