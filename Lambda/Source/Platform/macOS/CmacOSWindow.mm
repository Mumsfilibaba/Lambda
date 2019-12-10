#import "LambdaPch.h"
#if defined(LAMBDA_PLAT_MACOS)
    #import "CmacOSWindow.h"
    #import "Core/CLogManager.h"
    #import "Core/Event/CKeyEvent.h"
    #import "Core/Event/CWindowEvent.h"
    #import "Core/Event/CMouseEvent.h"

namespace Lambda
{
    static void GLFWErrorCallback(int, const char* pErrorString)
    {
        LOG_ENVIRONMENT_ERROR("GLFW: %s\n", pErrorString);
    }
    
    //------------
    //CmacOSWindow
    //------------
    
    bool CmacOSWindow::s_HasGLFW = false;
    
    CmacOSWindow::CmacOSWindow(const char* pTitle, uint32 width, uint32 height)
        : m_pWindow(nullptr),
        m_pCallback(nullptr),
        m_Width(0.0f),
        m_Height(0.0f),
        m_Fullscreen(false),
        m_HasFocus(false)
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
                LOG_ENVIRONMENT_INFO("macOS: Initialized GLFW\n");
            }
            else
            {
                LOG_ENVIRONMENT_INFO("macOS: Failed to initialize GLFW\n");
            }
        }
        
        //Setup window, we do not want any API since we are using vulkan
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
        
        //Create window
        m_pWindow = glfwCreateWindow(width, height, pTitle, nullptr, nullptr);
        if (m_pWindow)
        {
            LOG_ENVIRONMENT_INFO("macOS: Created window\n");
            
            //Setup size
            m_Width  = width;
            m_Height = height;
            
            //Setup callbacks
            glfwSetWindowCloseCallback(m_pWindow, CmacOSWindow::WindowClosedCallback);
            glfwSetWindowSizeCallback(m_pWindow, CmacOSWindow::WindowResizeCallback);
            glfwSetWindowPosCallback(m_pWindow, CmacOSWindow::WindowMoveCallback);
            glfwSetKeyCallback(m_pWindow, CmacOSWindow::KeyCallback);
            glfwSetCharCallback(m_pWindow, CmacOSWindow::TextCallback);
            glfwSetCursorPosCallback(m_pWindow, CmacOSWindow::MouseMoveCallback);
            glfwSetMouseButtonCallback(m_pWindow, CmacOSWindow::MouseButtonCallback);
            glfwSetScrollCallback(m_pWindow, CmacOSWindow::MouseScollCallback);
            glfwSetWindowFocusCallback(m_pWindow, CmacOSWindow::WindowFocusCallback);
            
            //Set userdata
            glfwSetWindowUserPointer(m_pWindow, this);
            
            //Set mouse input mode (TODO: Move to function)
            //glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            //Set fullscreen
            SetFullscreen(false);
        }
        
        LOG_ENVIRONMENT_INFO("Created window. w: %d, h: %d\n", width, height);
    }
    
    
    CmacOSWindow::~CmacOSWindow()
    {
        //Delete callback
        SafeDelete(m_pCallback);
        //Destroy glfw
        if (s_HasGLFW)
        {
            glfwTerminate();
        }
    }
    
    
    void CmacOSWindow::SetEventCallback(IEventCallback* pCallback)
    {
        SafeDelete(m_pCallback);
        m_pCallback = pCallback;
    }
    
    
    void CmacOSWindow::ProcessEvents() const
    {
        glfwPollEvents();
    }


    bool CmacOSWindow::HasFocus() const
    {
        return m_HasFocus;
    }
    
    
    uint32 CmacOSWindow::GetHeight() const
    {
        return m_Height;
    }
    
    
    uint32 CmacOSWindow::GetWidth() const
    {
        return m_Width;
    }
    
    
    void CmacOSWindow::WindowClosedCallback(GLFWwindow* pWindow)
    {
        CmacOSWindow* pUserWindow = reinterpret_cast<CmacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        CWindowClosedEvent event = CWindowClosedEvent();
        pUserWindow->DispatchEvent(event);
    }
    
    
    void CmacOSWindow::WindowResizeCallback(GLFWwindow* pWindow, int32 width, int32 height)
    {
        CmacOSWindow* pUserWindow = reinterpret_cast<CmacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        CWindowResizeEvent event = CWindowResizeEvent(uint32(width), uint32(height));
        pUserWindow->DispatchEvent(event);
    }
    
    
    void CmacOSWindow::WindowMoveCallback(GLFWwindow* pWindow, int32 x, int32 y)
    {
        CmacOSWindow* pUserWindow = reinterpret_cast<CmacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        CWindowMoveEvent event = CWindowMoveEvent(x, y);
        pUserWindow->DispatchEvent(event);
    }
    
    
    void CmacOSWindow::KeyCallback(GLFWwindow* pWindow, int32 key, int32 scancode, int32 action, int32 mods)
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
        CmacOSWindow* pUserWindow = reinterpret_cast<CmacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        if (action == GLFW_PRESS)
        {
            CKeyPressedEvent event = CKeyPressedEvent(KEY_UNKNOWN, 0, modifiers);
            pUserWindow->DispatchEvent(event);
        }
        else if (action == GLFW_REPEAT)
        {
            CKeyPressedEvent event = CKeyPressedEvent(KEY_UNKNOWN, 1, modifiers);
            pUserWindow->DispatchEvent(event);
        }
        else if (action == GLFW_RELEASE)
        {
            CKeyReleasedEvent event = CKeyReleasedEvent(KEY_UNKNOWN, modifiers);
            pUserWindow->DispatchEvent(event);
        }
    }
    
    
    void CmacOSWindow::TextCallback(GLFWwindow* pWindow, uint32 codepoint)
    {
        CmacOSWindow* pUserWindow = reinterpret_cast<CmacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        CKeyTypedEvent event = CKeyTypedEvent(codepoint);
        pUserWindow->DispatchEvent(event);
    }
    
    
    void CmacOSWindow::MouseMoveCallback(GLFWwindow* pWindow, double x, double y)
    {
        CmacOSWindow* pUserWindow = reinterpret_cast<CmacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        CMouseMovedEvent event = CMouseMovedEvent(int32(x), int32(y));
        pUserWindow->DispatchEvent(event);
    }
    
    
    void CmacOSWindow::MouseButtonCallback(GLFWwindow* pWindow, int32 button, int32 action, int32 mods)
    {
        //Convert button
        EMouseButton mouseButton = MOUSEBUTTON_UNKNOWN;
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
        CmacOSWindow* pUserWindow = reinterpret_cast<CmacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        if (action == GLFW_PRESS)
        {
            CMouseButtonPressedEvent event = CMouseButtonPressedEvent(mouseButton, modifiers);
            pUserWindow->DispatchEvent(event);
        }
        else if (action == GLFW_RELEASE)
        {
            CMouseButtonReleasedEvent event = CMouseButtonReleasedEvent(mouseButton, modifiers);
            pUserWindow->DispatchEvent(event);
        }
    }
    
    
    void CmacOSWindow::MouseScollCallback(GLFWwindow* pWindow, double xoffset, double yoffset)
    {
        CmacOSWindow* pUserWindow = reinterpret_cast<CmacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        CMouseScrolledEvent event = CMouseScrolledEvent(float(xoffset), float(yoffset));
        pUserWindow->DispatchEvent(event);
    }
    
    
    void CmacOSWindow::WindowFocusCallback(GLFWwindow* pWindow, int32 focused)
    {
        CmacOSWindow* pUserWindow = reinterpret_cast<CmacOSWindow*>(glfwGetWindowUserPointer(pWindow));
        
        //Set that we have focus
        pUserWindow->m_HasFocus = (focused != 0);
        
        //Tell rest of the system that the focus changed
        CWindowFocusChangedEvent event = CWindowFocusChangedEvent(pUserWindow->m_HasFocus);
        pUserWindow->DispatchEvent(event);
    }
    
    
    bool CmacOSWindow::SetFullscreen(bool fullscreen)
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
    
    
    bool CmacOSWindow::GetFullscreen() const
    {
        return m_Fullscreen;
    }
}
#endif
