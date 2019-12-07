#pragma once
#include "LambdaCore.h"
#include "CGamepad.h"

namespace Lambda
{
    //--------
    //CGamePad
    //--------

    class LAMBDA_API CGamePad : final
    {
    public:
        LAMBDA_STATIC_CLASS(CGamePad);
        
        static const CGamepadState& GetGamepadState();
    };
}
