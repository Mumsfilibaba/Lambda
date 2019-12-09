#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    class CEvent;

    //--------------
    //IEventCallback
    //--------------

    class LAMBDA_API IEventCallback
    {
    public:
        LAMBDA_INTERFACE(IEventCallback);
        virtual bool Call(const CEvent& event) = 0;
    };
}
