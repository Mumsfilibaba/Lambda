#pragma once
#include "LambdaCore.h"

#include "Core/Event/EventHandler.h"

#include "Platform/Common/CommonWindow.h"

namespace Lambda
{
    class CGame;
    class CEngine;
    class CTimestep;

    LAMBDA_API extern CEngine g_Engine;

    class LAMBDA_API CEngine final : IEventHandler
    {
    public:
        CEngine();
        ~CEngine() = default;
        
        LAMBDA_DECL_NO_COPY(CEngine);

        void Init();
        void Update(const CTimestep& deltaTime);
        void FixedUpdate(const CTimestep& deltaTime);
        void Render(const CTimestep& deltaTime);
        void Release();

        CCommonWindow& GetWindow()             { return *m_pWindow; }
        const CCommonWindow& GetWindow() const { return *m_pWindow; }

        static CEngine& Get() { return g_Engine; }
    public:
        /*IEventHandler*/
        virtual bool OnEvent(const SEvent& event) override final;
    private:
        CGame* m_pGame;
        CCommonWindow* m_pWindow;
    };
}
