#pragma once
#include "CSingleton.h"
#include "Event/EventCallback.h"

namespace Lambda
{
    class IWindow;

    //------------
    //CApplication
    //------------
    class LAMBDA_API CApplication : public CSingleton<CApplication>
    {
    public:
        CApplication();
        ~CApplication();
            
        virtual void ProcessEvents() = 0;
        virtual IWindow* GetNativeWindow() const = 0;
        
        virtual void Release();

        virtual void SetEventCallback(const CEventCallback& eventCallback)
        {
            m_EventCallback = eventCallback;
        }
    protected:
        virtual bool Initialize() = 0;
        virtual void DispatchEvent(const SSystemEvent& event);
    private:
        CEventCallback m_EventCallback;
    };
}
