#pragma once
#include "LambdaCore.h"
#if defined CreateWindow
#undef CreateWindow
#endif

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
        virtual uint32 GetHeight() const = 0;
        virtual uint32 GetWidth() const = 0;
#if defined(LAMBDA_DESKTOP)
    public:
        static IWindow* CreateWindow(const char* pTitle, uint32 uWidth, uint32 uHieght);
#endif
    };
}
