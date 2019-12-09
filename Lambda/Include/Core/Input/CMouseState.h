#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    //-----------
    //CMouseState
    //-----------

    class LAMBDA_API CMouseState final
    {
    public:
        CMouseState();
        CMouseState(const CKeyboardState& other);
        CMouseState(CKeyboardState&& other);
        ~CMouseState() = default;
        
        CMouseState& operator=(const CGamepadState& other);
        CMouseState& operator=(CGamepadState&& other);
    };
}
