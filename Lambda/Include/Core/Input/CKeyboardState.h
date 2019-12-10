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
        CKeyboardState() = default;
        CKeyboardState(const CKeyboardState& other) = default;
        CKeyboardState(CKeyboardState&& other) = default;
        ~CKeyboardState() = default;
        
        CKeyboardState& operator=(const CKeyboardState& other) = default;
        CKeyboardState& operator=(CKeyboardState&& other) = default;
        
        bool IsKeyDown(EKey key) {return false;}
        bool IsKeyUp(EKey key) {return false;}
        // Difference between key-down and -pressed is that pressed only checks the current frame's state
        bool IsKeyPressed(EKey key) {return false;}
	};
}
