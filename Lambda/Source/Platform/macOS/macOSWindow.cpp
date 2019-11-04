#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_MACOS)
    #include "../Vulkan/VKNDevice.h"
    #include "macOSWindow.h"
    #include "macOSInput.h"
    #include "Events/EventDispatcher.h"
    #include "Events/KeyEvent.h"
    #include "Events/WindowEvent.h"
    #include "Events/MouseEvent.h"
    #include "Graphics/Core/IDevice.h"
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
        return DBG_NEW macOSWindow(desc);
    }
    
    //-----------
    //macOSWindow
    //-----------
    
    bool macOSWindow::s_HasGLFW = false;
    
    macOSWindow::macOSWindow(const WindowDesc& desc)
        : m_pWindow(nullptr),
        m_pCallback(nullptr),
        m_Width(0.0f),
        m_Height(0.0f),
        m_Fullscreen(false),
        m_HasFocus(false)
    {
        Init(desc);
        LOG_SYSTEM_INFO("Created window. w: %d, h: %d\n", desc.Width, desc.Height);
    }
    
    
    macOSWindow::~macOSWindow()
    {
        //Delete callback
        SafeDelete(m_pCallback);
        //Destroy glfw
        if (s_HasGLFW)
        {
            glfwTerminate();
        }
    }
    
    
    void macOSWindow::Init(const WindowDesc& desc)
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
            glfwSetWindowCloseCallback(m_pWindow, macOSWindow::WindowClosedCallback);
            glfwSetWindowSizeCallback(m_pWindow, macOSWindow::WindowResizeCallback);
            glfwSetWindowPosCallback(m_pWindow, macOSWindow::WindowMoveCallback);
            glfwSetKeyCallback(m_pWindow, macOSWindow::KeyCallback);
            glfwSetCharCallback(m_pWindow, macOSWindow::TextCallback);
            glfwSetCursorPosCallback(m_pWindow, macOSWindow::MouseMoveCallback);
            glfwSetMouseButtonCallback(m_pWindow, macOSWindow::MouseButtonCallback);
            glfwSetScrollCallback(m_pWindow, macOSWindow::MouseScollCallback);
            glfwSetWindowFocusCallback(m_pWindow, macOSWindow::WindowFocusCallback);
            
            //Set userdata
            glfwSetWindowUserPointer(m_pWindow, this);
            
            //Set mouse input mode (TODO: Move to function)
            //glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            //Set fullscreen
            SetFullscreen(desc.Fullscreen);
        }
    }
    
    
    void macOSWindow::SetEventCallback(IEventCallback* pCallback)
    {
        SafeDelete(m_pCallback);
        m_pCallback = pCallback;
    }
    
    
    void macOSWindow::OnUpdate() const
    {
        glfwPollEvents();
    }


    bool macOSWindow::HasFocus() const
    {
        return m_HasFocus;
    }
    
    
    uint32 macOSWindow::GetHeight() const
    {
        return m_Height;
    }
    
    
    uint32 macOSWindow::GetWidth() const
    {
        return m_Width;
    }
    
    
    void* macOSWindow::GetNativeHandle() const
    {
        return m_pWindow;
    }
    
    
    void macOSWindow::WindowClosedCallback(GLFWwindow* pWindow)
    {
        macOSWindow* pUserWindow = reinterpret_cast<macOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        WindowClosedEvent event = WindowClosedEvent();
        pUserWindow->DispatchEvent(event);
    }
    
    
    void macOSWindow::WindowResizeCallback(GLFWwindow* pWindow, int32 width, int32 height)
    {
        macOSWindow* pUserWindow = reinterpret_cast<macOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        WindowResizeEvent event = WindowResizeEvent(uint32(width), uint32(height));
        pUserWindow->DispatchEvent(event);
    }
    
    
    void macOSWindow::WindowMoveCallback(GLFWwindow* pWindow, int32 x, int32 y)
    {
        macOSWindow* pUserWindow = reinterpret_cast<macOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        WindowMoveEvent event = WindowMoveEvent(x, y);
        pUserWindow->DispatchEvent(event);
    }
    
    
    void macOSWindow::KeyCallback(GLFWwindow* pWindow, int32 key, int32 scancode, int32 action, int32 mods)
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
        macOSWindow* pUserWindow = reinterpret_cast<macOSWindow*>(glfwGetWindowUserPointer(pWindow));
        if (action == GLFW_PRESS)
        {
            KeyPressedEvent event = KeyPressedEvent(macOSInput::ConvertGLFWKey(key), 0, modifiers);
            pUserWindow->DispatchEvent(event);
        }
        else if (action == GLFW_REPEAT)
        {
            KeyPressedEvent event = KeyPressedEvent(macOSInput::ConvertGLFWKey(key), 1, modifiers);
            pUserWindow->DispatchEvent(event);
        }
        else if (action == GLFW_RELEASE)
        {
            KeyReleasedEvent event = KeyReleasedEvent(macOSInput::ConvertGLFWKey(key), modifiers);
            pUserWindow->DispatchEvent(event);
        }
    }
    
    
    void macOSWindow::TextCallback(GLFWwindow* pWindow, uint32 codepoint)
    {
        macOSWindow* pUserWindow = reinterpret_cast<macOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        KeyTypedEvent event = KeyTypedEvent(codepoint);
        pUserWindow->DispatchEvent(event);
    }
    
    
    void macOSWindow::MouseMoveCallback(GLFWwindow* pWindow, double x, double y)
    {
        macOSWindow* pUserWindow = reinterpret_cast<macOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        MouseMovedEvent event = MouseMovedEvent(int32(x), int32(y));
        pUserWindow->DispatchEvent(event);
    }
    
    
    void macOSWindow::MouseButtonCallback(GLFWwindow* pWindow, int32 button, int32 action, int32 mods)
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
        macOSWindow* pUserWindow = reinterpret_cast<macOSWindow*>(glfwGetWindowUserPointer(pWindow));
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
    
    
    void macOSWindow::MouseScollCallback(GLFWwindow* pWindow, double xoffset, double yoffset)
    {
        macOSWindow* pUserWindow = reinterpret_cast<macOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        MouseScrolledEvent event = MouseScrolledEvent(float(xoffset), float(yoffset));
        pUserWindow->DispatchEvent(event);
    }
    
    
    void macOSWindow::WindowFocusCallback(GLFWwindow* pWindow, int32 focused)
    {
        macOSWindow* pUserWindow = reinterpret_cast<macOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        //Set that we have focus
        pUserWindow->m_HasFocus = (focused != 0);
        
        //Tell rest of the system that the focus changed
        WindowFocusChangedEvent event = WindowFocusChangedEvent(pUserWindow->m_HasFocus);
        pUserWindow->DispatchEvent(event);
    }
    
    
    bool macOSWindow::SetFullscreen(bool fullscreen)
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
    
    
    bool macOSWindow::GetFullscreen() const
    {
        return m_Fullscreen;
    }
    
    
    void macOSWindow::DispatchEvent(const Event &event)
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
