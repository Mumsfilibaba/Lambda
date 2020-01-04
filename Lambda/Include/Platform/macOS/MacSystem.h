#pragma once
#include "LambdaCore.h"

#if defined(LAMBDA_PLAT_MACOS)
    #include "Core/Engine/System.h"
    #include "Core/Event/SystemEventDispatcher.h"

namespace Lambda
{
    class IEngine;
    class IWindow;

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
        
        virtual bool CreateWindow(const char* pTitle, uint32 width, uint32 height) override final;

        virtual void ProcessSystemEvents() override final;
        virtual void AddEventListener(ISystemEventListener* pListener) override final;
        virtual void RemoveEventListener(ISystemEventListener* pListener) override final;

        virtual void Release() override final;

        virtual const IWindow* GetWindow() const override final { return m_pWindow; }
    private:
        IEngine* m_pEngine;
        IWindow* m_pWindow;
        CSystemEventDispatcher* m_pEventDispatcher;
    };
}
#endif
