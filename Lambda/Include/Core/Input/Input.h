#pragma once
#include "LambdaCore.h"
#include "Keyboard.h"
#include "Mouse.h"

namespace Lambda
{
    namespace Input
    {
        bool IsKeyUp(EKey key);
        bool IsKeyDown(EKey key);
        uint32 GetModiferKeys();
        
        bool IsMouseButtonUp(EMouseButton button);
        bool IsMouseButtonDown(EMouseButton button);
    
        void SetMousePosition(uint32 x, uint32 y);
        void GetMousePosition(uint32& x, uint32& y);
    }
}
