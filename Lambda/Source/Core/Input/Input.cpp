#include "LambdaPch.h"

#include "Core/Engine.h"
#include "Core/Input/Input.h"

#include "Platform/PlatformInput.h"

namespace Lambda
{
    namespace Input
    {
        void Init()
        {
            PlatformInput::Init();
        }
        
        void Update()
        {
            PlatformInput::Update();
        }

        bool IsKeyPressed(EKey key)
        {
            return PlatformInput::IsKeyPressed(key);
        }

        bool IsKeyReleased(EKey key)
        {
            return PlatformInput::IsKeyReleased(key);
        }

        bool IsModiferKeyActive(EKeyModifier modiferKey)
        {
            uint32 modifers = GetModiferKeys();
            return modifers & modiferKey;
        }

        uint32 GetModiferKeys()
        {
            return PlatformInput::GetModiferKeys();
        }
        
        bool IsMouseButtonPressed(EMouseButton button)
        {
            return PlatformInput::IsMouseButtonPressed(button);
        }

        bool IsMouseButtonReleased(EMouseButton button)
        {
            return PlatformInput::IsMouseButtonReleased(button);
        }

        void SetMousePosition(int32 x, int32 y)
        {   
            IWindow& window = CEngine::Get().GetWindow();
            PlatformInput::SetMousePosition(&window, x, y);
        }

        void GetMousePosition(int32& x, int32& y)
        {   
            IWindow& window = CEngine::Get().GetWindow();
            PlatformInput::GetMousePosition(&window, x, y);
        }
        
        void GetMouseState(SMouseState& mouseState)
        {
            mouseState = PlatformInput::GetMouseState();
        }

        void GetGamepadState(SGamepadState& gamepadState, EGamepad gamepad)
        {
            gamepadState = PlatformInput::GetGamepadState(gamepad);
        }
        
        void GetKeyboardState(SKeyboardState& keyboardState)
        {
            keyboardState = PlatformInput::GetKeyboardState();
        }
    }
}
