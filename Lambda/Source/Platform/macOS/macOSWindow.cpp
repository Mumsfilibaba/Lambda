#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_MACOS)
    #include "macOSWindow.h"
    #include "Graphics/IGraphicsDevice.h"
    #include "System/Input.h"
    #include "System/JoystickManager.h"
    #include "System/Log.h"

namespace Lambda
{
    //Log
    ILog* ILog::Create()
    {
        return nullptr;
    }
    
    
    //Input
    Input* Input::Create()
    {
        return nullptr;
    }
    
    
    //Joystickmanager
    JoystickManager* JoystickManager::Create()
    {
        return nullptr;
    }
    
    
    //Graphicsdevice
    IGraphicsDevice* IGraphicsDevice::s_pInstance = nullptr;
    
    IGraphicsDevice* IGraphicsDevice::Create(IWindow* pWindow, GraphicsContextFlags flags)
    {
        return nullptr;
    }
    
    
    IGraphicsDevice* IGraphicsDevice::GetInstance()
    {
        return s_pInstance;
    }
    
    
    bool IGraphicsDevice::OnEvent(const Event& event)
    {
        return (s_pInstance) ? s_pInstance->InternalOnEvent(event) : false;
    }
    
    
    //Window
    IWindow* IWindow::Create(const WindowDesc& desc)
    {
        return DBG_NEW MacOSWindow();
    }
    
    
    MacOSWindow::MacOSWindow()
    {
        
    }
    
    
    MacOSWindow::~MacOSWindow()
    {
        
    }
    
    
    void MacOSWindow::SetEventCallback(EventCallback callback)
    {
        
    }
    
    
    void MacOSWindow::OnUpdate() const
    {
        
    }
    
    
    uint32 MacOSWindow::GetHeight() const
    {
        return 0;
    }
    
    
    uint32 MacOSWindow::GetWidth() const
    {
        return 0;
    }
    
    
    void* MacOSWindow::GetNativeHandle() const
    {
        return nullptr;
    }
}
#endif
