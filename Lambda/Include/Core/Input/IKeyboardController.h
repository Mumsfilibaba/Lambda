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

		virtual bool IsKeyUp(EKey key) const = 0;
		virtual bool IsKeyDown(EKey key) const = 0;
        virtual CKeyboardState GetKeyboardState() const = 0;
    };
}
