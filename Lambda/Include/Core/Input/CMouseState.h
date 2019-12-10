#pragma once
#include "EMouseButton.h"

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
        
        CMouseState& operator=(const CMouseState& other);
        CMouseState& operator=(CMouseState&& other);
    private:
    };
}
