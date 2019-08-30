#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_MACOS)
    #include "macOSWindow.h"
    #include "macOSInput.h"
    #include "System/EventDispatcher.h"
    #include "Graphics/IGraphicsDevice.h"
    #include "System/Input.h"
    #include "System/JoystickManager.h"
    #include "System/Log.h"

namespace Lambda
{  
    IWindow* IWindow::Create(const WindowDesc& desc)
    {
        return DBG_NEW MacOSWindow(desc);
    }
    
    //Static variable declarations
    bool MacOSWindow::s_HasGLFW = false;
    
    //macOS window definition
    MacOSWindow::MacOSWindow(const WindowDesc& desc)
        : m_pWindow(nullptr),
        m_Width(0.0f),
        m_Height(0.0f),
        m_OnEvent(nullptr)
    {
        Init(desc);
        
        LOG_SYSTEM_INFO("Created window. w: %d, h: %d\n", desc.Width, desc.Height);
    }
    
    
    MacOSWindow::~MacOSWindow()
    {
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
        }
        else
        {
            LOG_DEBUG_ERROR("macOS: Failed to create window");
        }
    }
    
    
    void MacOSWindow::SetEventCallback(EventCallback callback)
    {
        m_OnEvent = callback;
    }
    
    
    void MacOSWindow::OnUpdate() const
    {
        glfwPollEvents();
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
        Event event;
        event.Type = EVENT_TYPE_WINDOW_CLOSED;
        
        MacOSWindow* pUserWindow = reinterpret_cast<MacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        pUserWindow->m_OnEvent(event);
    }
    
    
    void MacOSWindow::WindowResizeCallback(GLFWwindow* pWindow, int32 width, int32 height)
    {
        Event event;
        event.Type = EVENT_TYPE_WINDOW_RESIZE;
        event.WindowResize.Width = width;
        event.WindowResize.Height = height;
        
        MacOSWindow* pUserWindow = reinterpret_cast<MacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        pUserWindow->m_OnEvent(event);
    }
    
    
    void MacOSWindow::WindowMoveCallback(GLFWwindow* pWindow, int32 x, int32 y)
    {        
        Event event;
        event.Type = EVENT_TYPE_WINDOW_MOVE;
        event.WindowMove.PosX = x;
        event.WindowMove.PosY = y;
        
        MacOSWindow* pUserWindow = reinterpret_cast<MacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        pUserWindow->m_OnEvent(event);
    }
    
    
    void MacOSWindow::KeyCallback(GLFWwindow* pWindow, int32 key, int32 scancode, int32 action, int32 mods)
    {
        Event event;
        event.Type = (action == GLFW_PRESS) ? EVENT_TYPE_KEYDOWN : EVENT_TYPE_KEYUP;
        event.KeyEvent.RepeatCount = 1;
        event.KeyEvent.KeyCode = MacOSInput::ConvertGLFWKey(key);
        
        MacOSWindow* pUserWindow = reinterpret_cast<MacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        pUserWindow->m_OnEvent(event);
    }
    
    
    void MacOSWindow::TextCallback(GLFWwindow* pWindow, uint32 codepoint)
    {
        Event event;
        event.Type = EVENT_TYPE_TEXT;
        event.TextEvent.Character = codepoint;
        
        MacOSWindow* pUserWindow = reinterpret_cast<MacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        pUserWindow->m_OnEvent(event);
    }
    
    
    void MacOSWindow::MouseMoveCallback(GLFWwindow* pWindow, double x, double y)
    {
        Event event;
        event.Type = EVENT_TYPE_MOUSE_MOVED;
        event.MouseMoveEvent.PosX = int16(x);
        event.MouseMoveEvent.PosY = int16(y);
        
        MacOSWindow* pUserWindow = reinterpret_cast<MacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        pUserWindow->m_OnEvent(event);
    }
    
    
    void MacOSWindow::MouseButtonCallback(GLFWwindow* pWindow, int32 button, int32 action, int32 mods)
    {
        Event event;
        event.Type = (action == GLFW_PRESS) ? EVENT_TYPE_MOUSE_BUTTONDOWN : EVENT_TYPE_MOUSE_BUTTONUP;
        if (button == GLFW_MOUSE_BUTTON_RIGHT)
            event.MouseButtonEvent.Button = MOUSEBUTTON_RIGHT;
        else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
            event.MouseButtonEvent.Button = MOUSEBUTTON_MIDDLE;
        else if (button == GLFW_MOUSE_BUTTON_LEFT)
            event.MouseButtonEvent.Button = MOUSEBUTTON_LEFT;
        else if (button == GLFW_MOUSE_BUTTON_4)
            event.MouseButtonEvent.Button = MOUSEBUTTON_FORWARD;
        else if (button == GLFW_MOUSE_BUTTON_5)
            event.MouseButtonEvent.Button = MOUSEBUTTON_BACKWARD;
        
        MacOSWindow* pUserWindow = reinterpret_cast<MacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        pUserWindow->m_OnEvent(event);
    }
    
    
    void MacOSWindow::MouseScollCallback(GLFWwindow* pWindow, double xoffset, double yoffset)
    {
        MacOSWindow* pUserWindow = reinterpret_cast<MacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        //Vertical value
        Event event;
        event.Type = EVENT_TYPE_MOUSE_SCROLLED;
        if (yoffset != 0.0f)
        {
            event.MouseScrollEvent.Value = yoffset;
            event.MouseScrollEvent.Vertical = true;
            pUserWindow->m_OnEvent(event);
        }
        
        //Send another event for horizontal value
        if (xoffset != 0.0f)
        {
            event.MouseScrollEvent.Value = xoffset;
            event.MouseScrollEvent.Vertical = false;
            pUserWindow->m_OnEvent(event);
        }
    }
    
    
    void MacOSWindow::WindowFocusCallback(GLFWwindow* pWindow, int32 focused)
    {
        Event event;
        event.Type = EVENT_TYPE_FOCUS_CHANGED;
        event.FocusChanged.HasFocus = (focused != 0);
        
        MacOSWindow* pUserWindow = reinterpret_cast<MacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        pUserWindow->m_OnEvent(event);
    }
    
    
    IGraphicsDevice* MacOSWindow::GetGraphicsDevice() const
    {
        return nullptr;
    }
}
#endif
