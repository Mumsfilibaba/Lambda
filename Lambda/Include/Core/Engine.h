#pragma once
#include "LambdaCore.h"

#include "Platform/Generic/GenericWindow.h"

namespace Lambda
{
    class CEngine;
    LAMBDA_API extern CEngine g_Engine;

    class LAMBDA_API CEngine final
    {
    public:
        CEngine();
        ~CEngine() = default;
        
        LAMBDA_DECL_NO_COPY(CEngine);

        void Init();
        void Release();

        CGenericWindow& GetWindow()             { return *m_pWindow; }
        const CGenericWindow& GetWindow() const { return *m_pWindow; }

        static CEngine& Get() { return g_Engine; }
    private:
        CGenericWindow* m_pWindow;
    };
}
