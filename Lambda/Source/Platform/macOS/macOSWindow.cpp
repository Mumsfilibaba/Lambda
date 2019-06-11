#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_MACOS)
    #include "macOSWindow.h"
    #include "Graphics/IGraphicsDevice.h"
    #include "System/Input.h"
    #include "System/JoystickManager.h"
    #include "System/Log.h"

namespace Lambda
{  
    //Input
    Input* Input::Create()
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
        return DBG_NEW MacOSWindow(desc);
    }
    
    
    bool MacOSWindow::s_HasInitGLFW = false;
    
    MacOSWindow::MacOSWindow(const WindowDesc& desc)
    {
        Init(desc);
    }
    
    
    MacOSWindow::~MacOSWindow()
    {
        //Destroy glfw
        if (s_HasInitGLFW)
        {
            glfwTerminate();
        }
    }
    
    
    void MacOSWindow::Init(const WindowDesc& desc)
    {
        //Init glfw
        if (!s_HasInitGLFW)
        {
            if (glfwInit())
            {
                s_HasInitGLFW = true;
                LOG_DEBUG_INFO("macOS: Initialized GLFW\n");
            }
            else
            {
                LOG_DEBUG_INFO("macOS: Failed to initialize GLFW\n");
            }
        }
        
        //Create window
        m_pWindow = glfwCreateWindow(desc.Width, desc.Height, desc.pTitle, nullptr, nullptr);
        if (m_pWindow)
        {
            LOG_DEBUG_INFO("macOS: Created window\n");
            
            m_Width     = desc.Width;
            m_Height    = desc.Height;
        }
        else
        {
            LOG_DEBUG_ERROR("macOS: Failed to create window");
        }
    }
    
    
    void MacOSWindow::SetEventCallback(EventCallback callback)
    {
        
    }
    
    
    void MacOSWindow::OnUpdate() const
    {
        glfwPollEvents();
        glfwWindowShouldClose(m_pWindow);
    }
    
    
    uint32 MacOSWindow::GetHeight() const
    {
        return m_Height;
    }
    
    
    uint32 MacOSWindow::GetWidth() const
    {
        return m_Width;
    }
    
    
    void* MacOSWindow::GetNativeHandle() const
    {
        return m_pWindow;
    }
}
#endif
