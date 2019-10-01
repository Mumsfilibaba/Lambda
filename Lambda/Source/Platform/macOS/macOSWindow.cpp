#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_MACOS)
    #include "../Vulkan/VKNDevice.h"
    #include "macOSWindow.h"
    #include "macOSInput.h"
    #include "Events/EventDispatcher.h"
    #include "Events/KeyEvent.h"
    #include "Events/WindowEvent.h"
    #include "Events/MouseEvent.h"
    #include "Graphics/IDevice.h"
    #include "System/Input.h"
    #include "System/JoystickManager.h"
    #include "System/Log.h"

namespace Lambda
{
    static void GLFWErrorCallback(int, const char* pErrorString)
    {
        LOG_DEBUG_ERROR("GLFW: %s\n", pErrorString);
    }

    //-------
    //IWindow
    //-------
    
    IWindow* IWindow::Create(const WindowDesc& desc)
    {
        return DBG_NEW MacOSWindow(desc);
    }
    
    //-----------
    //MacOSWindow
    //-----------
    
    bool MacOSWindow::s_HasGLFW = false;
    
    MacOSWindow::MacOSWindow(const WindowDesc& desc)
        : m_pWindow(nullptr),
        m_Device(nullptr),
        m_Width(0.0f),
        m_Height(0.0f),
        m_pCallback(nullptr),
        m_Fullscreen(false),
        m_HasFocus(false)
    {
        Init(desc);
        LOG_SYSTEM_INFO("Created window. w: %d, h: %d\n", desc.Width, desc.Height);
    }
    
    
    MacOSWindow::~MacOSWindow()
    {
        //Delete callback
        SafeDelete(m_pCallback);
        //Destroy glfw
        if (s_HasGLFW)
        {
            glfwTerminate();
        }
    }
    
    
    void MacOSWindow::Init(const WindowDesc& desc)
    {
        //Init glfw
        if (!s_HasGLFW)
        {
            //Init error callback
            glfwSetErrorCallback(GLFWErrorCallback);
            
            //Init GLFW
            if (glfwInit())
            {
                s_HasGLFW = true;
                LOG_DEBUG_INFO("macOS: Initialized GLFW\n");
            }
            else
            {
                LOG_DEBUG_INFO("macOS: Failed to initialize GLFW\n");
            }
        }
        
        //Setup window, we do not want any API since we are using vulkan
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
        
        //Create window
        m_pWindow = glfwCreateWindow(desc.Width, desc.Height, desc.pTitle, nullptr, nullptr);
        if (m_pWindow)
        {
            LOG_DEBUG_INFO("macOS: Created window\n");
            
            //Setup size
            m_Width = desc.Width;
            m_Height = desc.Height;
            
            //Setup callbacks
            glfwSetWindowCloseCallback(m_pWindow, MacOSWindow::WindowClosedCallback);
            glfwSetWindowSizeCallback(m_pWindow, MacOSWindow::WindowResizeCallback);
            glfwSetWindowPosCallback(m_pWindow, MacOSWindow::WindowMoveCallback);
            glfwSetKeyCallback(m_pWindow, MacOSWindow::KeyCallback);
            glfwSetCharCallback(m_pWindow, MacOSWindow::TextCallback);
            glfwSetCursorPosCallback(m_pWindow, MacOSWindow::MouseMoveCallback);
            glfwSetMouseButtonCallback(m_pWindow, MacOSWindow::MouseButtonCallback);
            glfwSetScrollCallback(m_pWindow, MacOSWindow::MouseScollCallback);
            glfwSetWindowFocusCallback(m_pWindow, MacOSWindow::WindowFocusCallback);
            
            //Set userdata
            glfwSetWindowUserPointer(m_pWindow, this);
            
            //Set mouse input mode (TODO: Move to function)
            //glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            //Set fullscreen
            SetFullscreen(desc.Fullscreen);
            
            //Create graphics context
            {
                DeviceDesc gdDesc = {};
                gdDesc.pWindow          = this;
                gdDesc.VerticalSync     = desc.VerticalSync;
                gdDesc.BackBufferCount  = 3;
                gdDesc.Api              = desc.GraphicsDeviceAPI;
                gdDesc.SampleCount      = desc.SampleCount;
#if LAMBDA_DEBUG
                gdDesc.Flags = GRAPHICS_CONTEXT_FLAG_DEBUG;
#else
                gdDesc.Flags = GRAPHICS_CONTEXT_FLAG_NONE;
#endif
                if (desc.GraphicsDeviceAPI == GRAPHICS_API_D3D12)
                {
                    LOG_DEBUG_ERROR("Lambda Engine: D3D12 is only supported on the Windows-Platform\n");
                    m_Device = nullptr;
                }
                else if (desc.GraphicsDeviceAPI == GRAPHICS_API_VULKAN)
                {
                    m_Device = AutoRef<IDevice>(DBG_NEW VKNDevice(gdDesc));
                }
                else
                {
                    LOG_DEBUG_ERROR("Lambda Engine: Unsupported graphics API specified\n");
                }
            }
        }
        else
        {
            LOG_DEBUG_ERROR("macOS: Failed to create window");
        }
    }
    
    
    void MacOSWindow::SetEventCallback(IEventCallback* pCallback)
    {
        if (m_pCallback != nullptr)
        {
            SafeDelete(m_pCallback);
        }
        m_pCallback = pCallback;
    }
    
    
    void MacOSWindow::OnUpdate() const
    {
        glfwPollEvents();
    }


    bool MacOSWindow::HasFocus() const
    {
        return m_HasFocus;
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
    
    
    void MacOSWindow::WindowClosedCallback(GLFWwindow* pWindow)
    {
        MacOSWindow* pUserWindow = reinterpret_cast<MacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        WindowClosedEvent event = WindowClosedEvent();
        pUserWindow->DispatchEvent(event);
    }
    
    
    void MacOSWindow::WindowResizeCallback(GLFWwindow* pWindow, int32 width, int32 height)
    {
        MacOSWindow* pUserWindow = reinterpret_cast<MacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        WindowResizeEvent event = WindowResizeEvent(uint32(width), uint32(height));
        pUserWindow->DispatchEvent(event);
    }
    
    
    void MacOSWindow::WindowMoveCallback(GLFWwindow* pWindow, int32 x, int32 y)
    {
        MacOSWindow* pUserWindow = reinterpret_cast<MacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        WindowMoveEvent event = WindowMoveEvent(x, y);
        pUserWindow->DispatchEvent(event);
    }
    
    
    void MacOSWindow::KeyCallback(GLFWwindow* pWindow, int32 key, int32 scancode, int32 action, int32 mods)
    {
        //Convert modifiers
        uint32 modifiers = 0;
        if (mods & GLFW_MOD_SHIFT)
            modifiers |= KEY_MODIFIER_SHIFT;
        if (mods & GLFW_MOD_CONTROL)
            modifiers |= KEY_MODIFIER_CONTROL;
        if (mods & GLFW_MOD_ALT)
            modifiers |= KEY_MODIFIER_ALT;
        if (mods & GLFW_MOD_SUPER)
            modifiers |= KEY_MODIFIER_SUPER;
        if (mods & GLFW_MOD_CAPS_LOCK)
            modifiers |= KEY_MODIFIER_CAPS_LOCK;
        if (mods & GLFW_MOD_NUM_LOCK)
            modifiers |= KEY_MODIFIER_NUM_LOCK;
        
        //Dispatch events
        MacOSWindow* pUserWindow = reinterpret_cast<MacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        if (action == GLFW_PRESS)
        {
            KeyPressedEvent event = KeyPressedEvent(MacOSInput::ConvertGLFWKey(key), 0, modifiers);
            pUserWindow->DispatchEvent(event);
        }
        else if (action == GLFW_REPEAT)
        {
            KeyPressedEvent event = KeyPressedEvent(MacOSInput::ConvertGLFWKey(key), 1, modifiers);
            pUserWindow->DispatchEvent(event);
        }
        else if (action == GLFW_RELEASE)
        {
            KeyReleasedEvent event = KeyReleasedEvent(MacOSInput::ConvertGLFWKey(key), modifiers);
            pUserWindow->DispatchEvent(event);
        }
    }
    
    
    void MacOSWindow::TextCallback(GLFWwindow* pWindow, uint32 codepoint)
    {
        MacOSWindow* pUserWindow = reinterpret_cast<MacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        KeyTypedEvent event = KeyTypedEvent(codepoint);
        pUserWindow->DispatchEvent(event);
    }
    
    
    void MacOSWindow::MouseMoveCallback(GLFWwindow* pWindow, double x, double y)
    {
        MacOSWindow* pUserWindow = reinterpret_cast<MacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        MouseMovedEvent event = MouseMovedEvent(int32(x), int32(y));
        pUserWindow->DispatchEvent(event);
    }
    
    
    void MacOSWindow::MouseButtonCallback(GLFWwindow* pWindow, int32 button, int32 action, int32 mods)
    {
        //Convert button
        MouseButton mouseButton = MOUSEBUTTON_UNKNOWN;
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
            mouseButton = MOUSEBUTTON_RIGHT;
        else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
            mouseButton = MOUSEBUTTON_MIDDLE;
        else if (button == GLFW_MOUSE_BUTTON_LEFT)
            mouseButton = MOUSEBUTTON_LEFT;
        else if (button == GLFW_MOUSE_BUTTON_4)
            mouseButton = MOUSEBUTTON_FORWARD;
        else if (button == GLFW_MOUSE_BUTTON_5)
            mouseButton = MOUSEBUTTON_BACKWARD;
        
        //Convert modifiers
        uint32 modifiers = 0;
        if (mods & GLFW_MOD_SHIFT)
            modifiers |= KEY_MODIFIER_SHIFT;
        if (mods & GLFW_MOD_CONTROL)
            modifiers |= KEY_MODIFIER_CONTROL;
        if (mods & GLFW_MOD_ALT)
            modifiers |= KEY_MODIFIER_ALT;
        if (mods & GLFW_MOD_SUPER)
            modifiers |= KEY_MODIFIER_SUPER;
        if (mods & GLFW_MOD_CAPS_LOCK)
            modifiers |= KEY_MODIFIER_CAPS_LOCK;
        if (mods & GLFW_MOD_NUM_LOCK)
            modifiers |= KEY_MODIFIER_NUM_LOCK;
    
        //Dispatch event
        MacOSWindow* pUserWindow = reinterpret_cast<MacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        if (action == GLFW_PRESS)
        {
            MouseButtonPressedEvent event = MouseButtonPressedEvent(mouseButton, modifiers);
            pUserWindow->DispatchEvent(event);
        }
        else if (action == GLFW_RELEASE)
        {
            MouseButtonReleasedEvent event = MouseButtonReleasedEvent(mouseButton, modifiers);
            pUserWindow->DispatchEvent(event);
        }
    }
    
    
    void MacOSWindow::MouseScollCallback(GLFWwindow* pWindow, double xoffset, double yoffset)
    {
        MacOSWindow* pUserWindow = reinterpret_cast<MacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        MouseScrolledEvent event = MouseScrolledEvent(float(xoffset), float(yoffset));
        pUserWindow->DispatchEvent(event);
    }
    
    
    void MacOSWindow::WindowFocusCallback(GLFWwindow* pWindow, int32 focused)
    {
        MacOSWindow* pUserWindow = reinterpret_cast<MacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        //Set that we have focus
        pUserWindow->m_HasFocus = (focused != 0);
        
        //Tell rest of the system that the focus changed
        WindowFocusChangedEvent event = WindowFocusChangedEvent(pUserWindow->m_HasFocus);
        pUserWindow->DispatchEvent(event);
    }
    
    
    const IDevice* MacOSWindow::GetGraphicsDevice() const
    {
        return m_Device.Get();
    }
    
    
    bool MacOSWindow::SetFullscreen(bool fullscreen)
    {
        //glfwMaximizeWindow(m_pWindow);
        if (m_Fullscreen == fullscreen)
        {
            return true;
        }
        
        if (fullscreen)
        {
            GLFWmonitor* pMonitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* pVidMode = glfwGetVideoMode(pMonitor);
            glfwSetWindowMonitor(m_pWindow, pMonitor, 0, 0, m_Width, m_Height, pVidMode->refreshRate);
            
            m_Fullscreen = true;
        }
        else
        {
            glfwSetWindowMonitor(m_pWindow, nullptr, 0, 0, m_Width, m_Height, GLFW_DONT_CARE);
            m_Fullscreen = false;
        }
        
        
        return true;
    }
    
    
    bool MacOSWindow::GetFullscreen() const
    {
        return m_Fullscreen;
    }
    
    
    void MacOSWindow::DispatchEvent(const Event &event)
    {
        if (m_pCallback)
        {
            //When a eventhandler is registered, then we handled all the backloged items
            /*if (m_EventBackLog.size() > 0)
            {
                for (auto& e : m_EventBackLog)
                    m_EventCallback(e);
                
                m_EventBackLog.clear();
            }*/
            m_pCallback->Callback(event);
        }
        else
        {
            //If a eventcallback is not registered then we put the event in the backlog
            //m_EventBackLog.push_back(event);
        }
    }
}
#endif
