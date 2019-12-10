#pragma once
#include "EKey.h"

namespace Lambda
{
    //--------------
    //CKeyboardState
    //--------------

	class LAMBDA_API CKeyboardState final
	{
    public:
        CKeyboardState();
        CKeyboardState(const CKeyboardState& other);
        CKeyboardState(CKeyboardState&& other);
        ~CKeyboardState() = default;
        
        CKeyboardState& operator=(const CKeyboardState& other);
        CKeyboardState& operator=(CKeyboardState&& other);
        
        bool IsKeyDown(EKey key);
        bool IsKeyUp(EKey key);
        // Difference between key-down and -pressed is that pressed only checks the current frame's state
        bool IsKeyPressed(EKey key);
	};
}
