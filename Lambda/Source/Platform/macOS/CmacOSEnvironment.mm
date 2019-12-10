#import "LambdaPch.h"
#import "CmacOSEnvironment.h"
#import "Core/Event/CEventCallback.h"

namespace Lambda
{
    CmacOSEnvironment::CmacOSEnvironment()
        : CEnvironment(),
        m_pWindow(nullptr)
    {
    }


    CmacOSEnvironment::~CmacOSEnvironment()
    {
    }


    void CmacOSEnvironment::Init()
    {
        //Create window
        m_pWindow = DBG_NEW CmacOSWindow("Lambda Engine", 1280, 720);
        //Set environment on event as callback
        IEventCallback* pCallback = DBG_NEW CObjectEventCallback<CmacOSEnvironment, CEvent>(this, &CmacOSEnvironment::OnEvent);
        m_pWindow->SetEventCallback(pCallback);
    }


    void CmacOSEnvironment::ProcessEvents()
    {
        m_pWindow->ProcessEvents();
    }


    void CmacOSEnvironment::Release()
    {
        delete this;
    }


    void CmacOSEnvironment::PrintF(const char* pFormat, ...)
    {
        va_list args;
        va_start(args, pFormat);
        vprintf(pFormat, args);
        va_end(args);
    }
}
