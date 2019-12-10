#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    class IEventCallback;

    //-------
    //IWindow
    //-------
    
    class LAMBDA_API IWindow
    {
    public:
        LAMBDA_INTERFACE(IWindow);

        virtual bool HasFocus() const = 0;
        virtual void SetEventCallback(IEventCallback* pEventCallback) = 0;
        virtual uint32 GetHeight() const = 0;
        virtual uint32 GetWidth() const = 0;
    };
}
