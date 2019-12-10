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
        CMouseState()= default;
        CMouseState(const CMouseState& other)= default;
        CMouseState(CMouseState&& other)= default;
        ~CMouseState() = default;
        
        CMouseState& operator=(const CMouseState& other) = default;
        CMouseState& operator=(CMouseState&& other) = default;
    private:
    };
}
