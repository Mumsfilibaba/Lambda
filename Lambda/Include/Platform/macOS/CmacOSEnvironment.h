#pragma once
#if defined(LAMBDA_PLAT_MACOS)
    #include "Core/CEnvironment.h"
    #include "CmacOSWindow.h"

namespace Lambda
{
    class CWindowClosedEvent;

    //-----------------
    //CmacOSEnvironment
    //-----------------

    class CmacOSEnvironment : public CEnvironment
    {
    public:
        CmacOSEnvironment();
        ~CmacOSEnvironment();

        virtual void Initialize() override final;
        virtual void ProcessEvents() override final;
        virtual void Release() override final;
    private:
        virtual bool OnEvent(const CEvent& event) override final;
        
        bool OnWindowClosed(const CWindowClosedEvent& event);
        void CreateControllers();
        void CreateWindow();
    private:
        CmacOSWindow* m_pWindow;
    };
}
#endif
