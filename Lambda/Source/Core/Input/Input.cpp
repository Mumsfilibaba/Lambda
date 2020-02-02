#include "LambdaPch.h"

#include "Core/Input/Input.h"

namespace Lambda
{
    namespace Input
    {
        bool IsKeyUp(EKey key)
        {
            return false;
        }

        bool IsKeyDown(EKey key)
        {
            return false;
        }

        uint32 GetModiferKeys()
        {
            return 0;
        }
        
        bool IsMouseButtonUp(EMouseButton button)
        {
            return false;
        }

        bool IsMouseButtonDown(EMouseButton button)
        {
            return false;
        }
    
        void SetMousePosition(uint32 x, uint32 y)
        {
            
        }

        void GetMousePosition(uint32& x, uint32& y)
        {
            
        }
    }
}
