#import "LambdaPch.h"
#import "CmacOSEnvironment.h"
#import "Core/Event/CWindowEvent.h"
#import "Core/Event/CQuitEvent.h"
#import "Core/Event/CEventCallback.h"
#import "Core/Event/CEventDispatcher.h"
#import "Core/Input/CKeyboardController.h"
#import "CmacOSMouseController.h"

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


    void CmacOSEnvironment::Initialize()
    {
        //Allways call parent first
        CEnvironment::Initialize();
        
        CreateControllers();
        CreateWindow();
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


    void CmacOSEnvironment::CreateControllers()
    {
        //Create keyboardcontroller
        CKeyboardController* pKeyboardController = DBG_NEW CKeyboardController(this);
        SetKeyboardController(pKeyboardController);
        AddEventListener(pKeyboardController);
        
        //Create mousecontroller
        CmacOSMouseController* pMouseController = DBG_NEW CmacOSMouseController(this);
        SetMouseController(pMouseController);
        AddEventListener(pMouseController);
        
        CreateDefaultInputControllers();
    }


    void CmacOSEnvironment::CreateWindow()
    {
        m_pWindow = DBG_NEW CmacOSWindow("Lambda Engine", 1280, 720);
        
        //Set environment on event as callback
        IEventCallback* pCallback = DBG_NEW CObjectEventCallback<CmacOSEnvironment, CEvent>(this, &CmacOSEnvironment::OnEvent);
        m_pWindow->SetEventCallback(pCallback);
    }


    bool CmacOSEnvironment::OnEvent(const CEvent& event)
    {
        CEventDispatcher dispatcher;
        dispatcher.Dispatch(this, &CmacOSEnvironment::OnWindowClosed, event);
        return CEnvironment::OnEvent(event);
    }


    bool CmacOSEnvironment::OnWindowClosed(const CWindowClosedEvent&)
    {
        CQuitEvent quitEvent = CQuitEvent(0);
        OnEvent(quitEvent);
        return true;
    }
}
