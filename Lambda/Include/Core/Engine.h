#pragma once
#include "LambdaCore.h"
#include "Game.h"

#include "Time/Clock.h"
#include "Event/IEventListener.h"

namespace Lambda
{
    class IWindow;
    class IConsoleOutput;
    class CEngine;
    class CTimestep;

    LAMBDA_API extern CEngine g_Engine;

    class LAMBDA_API CEngine final : public IEventListener
    {
    public:
        CEngine();
        ~CEngine() = default;
        
        LAMBDA_DECL_NO_COPY(CEngine);

        void PreInit();
        void Init(CreateGameInstanceFunc pfnCreateGameInstanceFunc);
        void Start();
        void Tick();
        void RequestExit();
        void Release();
        void PostRelease();

        _forceinline CGame& GetGame() const
        { 
            LAMBDA_ASSERT_PRINT(m_pWindow, "Game instance not created properly");
            return *m_pGame; 
        }

        _forceinline IWindow& GetWindow() const 
        { 
            LAMBDA_ASSERT_PRINT(m_pWindow, "Window not initialized");
            return *m_pWindow; 
        }

        _forceinline IConsoleOutput* GetConsoleOutput() const 
        { 
            return m_pConsoleOutput; 
        }

        _forceinline bool IsRunning() const 
        { 
            return m_bIsRunning; 
        }

        static CEngine& Get() { return g_Engine; }
    public:
        /*IEventHandler*/
        virtual bool OnEvent(const SEvent& event) override final;
    private:
        CClock m_Frameclock;
        CGame* m_pGame;
        IWindow* m_pWindow;
        IConsoleOutput* m_pConsoleOutput;
        bool m_bIsRunning;
    };
}
