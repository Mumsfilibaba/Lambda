#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    class LAMBDA_API CEngine final
    {
    public:
        CEngine();
        ~CEngine() = default;
        
        void Init();
        void Release();
    private:
        //TODO: Insert Window Variable here
    };

    extern CEngine g_Engine;
}
