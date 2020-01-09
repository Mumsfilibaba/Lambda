#pragma once
#include "LambdaCore.h"

#if defined(LAMBDA_PLAT_MACOS)
    #include "Core/Engine/System.h"
    #include "Core/Event/SystemEventDispatcher.h"

    #include "MacWindow.h"
    #include "MacAppDelegate.h"

namespace Lambda
{
    //---------
    //MacSystem
    //---------

    class MacSystem final : public ISystem
    {
    public:       
        /*ISystem Interface*/
        virtual IWindow CreateWindow(const char* pTitle, uint32 nWidth, uint32 nHeight) override final;
        virtual void ProcessSystemEvents() override final;

        virtual void AddListener(ISystemEventListener* pListener) override final
        {
            m_EventDispatcher.AddListener(pListener);
        }

        virtual void RemoveListener(ISystemEventListener* pListener) override final
        {
            m_EventDispatcher.RemoveListener(pListener);
        }
        
        /*MacSystem Interface*/
        void OnSystemEvent(const SystemEvent& event);

        void SetHasFinishedLaunching(bool bHasFinishedLaunching) { m_bHasFinishedLaunching = bHasFinishedLaunching; }
        
        bool ShouldExit() const { return m_bShouldExit; }
        bool HasFinishedLaunching() const { return m_bHasFinishedLaunching; }
    private:
        MacSystem();
        ~MacSystem();

        void CreateMenuBar();
    private:
        CMacAppDelegate* m_pAppDelegate;
        
        SystemEventDispatcher m_EventDispatcher;
        bool m_bHasFinishedLaunching;
        bool m_bShouldExit;
    };
}
#endif
