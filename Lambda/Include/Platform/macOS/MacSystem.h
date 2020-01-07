#pragma once
#include "LambdaCore.h"

#if defined(LAMBDA_PLAT_MACOS)
    #include "Core/Engine/System.h"
    #include "Core/Event/SystemEventDispatcher.h"

    #include "MacWindow.h"
    #include "MacAppDelegate.h"

namespace Lambda
{
    class IEngine;

    //----------
    //CMacSystem
    //----------
    class CMacSystem final : public ISystem
    {
    public:
        CMacSystem(IEngine* pEngine);
        ~CMacSystem();
        
        /*ISystem Interface*/
        virtual bool Initialize() override final;
        
        virtual bool CreateWindow(const char* pTitle, uint32 nWidth, uint32 nHeight) override final;

        virtual void ProcessSystemEvents() override final;
        virtual void AddEventListener(ISystemEventListener* pListener) override final;
        virtual void RemoveEventListener(ISystemEventListener* pListener) override final;

        virtual void Release() override final;

        virtual const IWindow* GetWindow() const override final { return m_pWindow; }
        
        /*CMacSystem Interface*/
        void OnSystemEvent(const SSystemEvent& event);
        void OnWindowClose(CMacWindow* pWindow);

        void SetHasFinishedLaunching(bool bHasFinishedLaunching) { m_bHasFinishedLaunching = bHasFinishedLaunching; }
        
        bool ShouldExit() const { return m_bShouldExit; }
        bool HasFinishedLaunching() const { return m_bHasFinishedLaunching; }
    private:
        void CreateMenuBar();
    private:
        IEngine* m_pEngine;
        CMacWindow* m_pWindow;
        CMacAppDelegate* m_pAppDelegate;
        CSystemEventDispatcher* m_pEventDispatcher;
        
        bool m_bHasFinishedLaunching;
        bool m_bShouldExit;
    };
}
#endif
