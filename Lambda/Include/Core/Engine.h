#pragma once
#include "LambdaCore.h"

#include "Core/Event/EventHandler.h"

namespace Lambda
{
    class CGame;
    class CEngine;
    class IWindow;
    class CTimestep;

    LAMBDA_API extern CEngine g_Engine;

    class LAMBDA_API CEngine final : IEventListener
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

        CGame& GetGame() { return *m_pGame; }
        const CGame& GetGame() const { return *m_pGame; }
        
        IWindow& GetWindow()             { return *m_pWindow; }
        const IWindow& GetWindow() const { return *m_pWindow; }

        static CEngine& Get() { return g_Engine; }
    public:
        /*IEventHandler*/
        virtual bool OnEvent(const SEvent& event) override final;
    private:
        CGame* m_pGame;
        IWindow* m_pWindow;
    };
}
