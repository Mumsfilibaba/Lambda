#pragma once
#include "CKeyboardState.h"

namespace Lambda
{
    //-------------------
    //IKeyboardController
    //-------------------

    class IKeyboardController
    {
    public:
        LAMBDA_INTERFACE(IKeyboardController);
        virtual CKeyboardState GetKeyboardState() const = 0;
    };
}
