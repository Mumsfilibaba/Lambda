#pragma once
#include "Keyboard.h"

namespace Lambda
{
    struct SKeyboardState
    {
    public:
        _forceinline SKeyboardState()
        {
            memset(this, 0, sizeof(SKeyboardState));
        }

        _forceinline bool IsKeyReleased(EKey key) const
        {
            return bKeyStates[key] == false;
        }

        _forceinline bool IsKeyPressed(EKey key) const
        {
            return bKeyStates[key] == true;
        }
    public:
        bool bKeyStates[EKey::KEY_COUNT];
    };
}
