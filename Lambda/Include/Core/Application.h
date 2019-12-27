#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    class IWindow;
    class CEventCallback;

    //-----------
    //Application
    //-----------
    namespace Application
    {
        bool LAMBDA_API Initialize();
        void LAMBDA_API ProcessEvents();
        void LAMBDA_API Release();

        void LAMBDA_API SetEventCallback(const CEventCallback& eventCallback);

        IWindow* LAMBDA_API GetNativeWindow();
    };
}
