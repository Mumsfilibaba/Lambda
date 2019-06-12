#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_MACOS)
    #include "macOSWindow.h"
    #include "System/EventDispatcher.h"
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
    
    IGraphicsDevice* IGraphicsDevice::Create(IWindow* pWindow, const GraphicsDeviceDesc& desc)
    {
        if (desc.Api == GRAPHICS_API_D3D12)
        {
            LOG_DEBUG_ERROR("Lambda Engine: D3D12 is only supported on the Windows-Platform\n");
            return nullptr;
        }
        else if (desc.Api == GRAPHICS_API_VULKAN)
        {
            LOG_DEBUG_ERROR("Lambda Engine: Creating GraphicsDevice\n");
            return nullptr;
        }
        
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
        : m_pWindow(nullptr),
        m_Width(0),
        m_Height(0),
        m_OnEvent(nullptr)
    {
        Init(desc);
        
        LOG_SYSTEM_INFO("Creating window. w: %d, h: %d\n", desc.Width, desc.Height);
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
            
            //Setup size
            m_Width     = desc.Width;
            m_Height    = desc.Height;
            
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
        
        switch (key)
        {
        case GLFW_KEY_APOSTROPHE: event.KeyEvent.KeyCode = KEY_APOSTROPHE; break;
        case GLFW_KEY_SPACE: event.KeyEvent.KeyCode = KEY_SPACE; break;
        case GLFW_KEY_COMMA: event.KeyEvent.KeyCode = KEY_COMMA; break;
        case GLFW_KEY_MINUS: event.KeyEvent.KeyCode = KEY_MINUS; break;
        case GLFW_KEY_PERIOD: event.KeyEvent.KeyCode = KEY_PERIOD; break;
        case GLFW_KEY_SLASH: event.KeyEvent.KeyCode = KEY_SLASH; break;
        case GLFW_KEY_0: event.KeyEvent.KeyCode = KEY_0; break;
        case GLFW_KEY_1: event.KeyEvent.KeyCode = KEY_1; break;
        case GLFW_KEY_2: event.KeyEvent.KeyCode = KEY_2; break;
        case GLFW_KEY_3: event.KeyEvent.KeyCode = KEY_3; break;
        case GLFW_KEY_4: event.KeyEvent.KeyCode = KEY_4; break;
        case GLFW_KEY_5: event.KeyEvent.KeyCode = KEY_5; break;
        case GLFW_KEY_6: event.KeyEvent.KeyCode = KEY_6; break;
        case GLFW_KEY_7: event.KeyEvent.KeyCode = KEY_7; break;
        case GLFW_KEY_8: event.KeyEvent.KeyCode = KEY_8; break;
        case GLFW_KEY_9: event.KeyEvent.KeyCode = KEY_9; break;
        case GLFW_KEY_SEMICOLON: event.KeyEvent.KeyCode = KEY_SEMICOLON; break;
        case GLFW_KEY_EQUAL: event.KeyEvent.KeyCode = KEY_EQUAL; break;
        case GLFW_KEY_A: event.KeyEvent.KeyCode = KEY_A; break;
        case GLFW_KEY_B: event.KeyEvent.KeyCode = KEY_B; break;
        case GLFW_KEY_C: event.KeyEvent.KeyCode = KEY_C; break;
        case GLFW_KEY_D: event.KeyEvent.KeyCode = KEY_D; break;
        case GLFW_KEY_E: event.KeyEvent.KeyCode = KEY_E; break;
        case GLFW_KEY_F: event.KeyEvent.KeyCode = KEY_F; break;
        case GLFW_KEY_G: event.KeyEvent.KeyCode = KEY_G; break;
        case GLFW_KEY_H: event.KeyEvent.KeyCode = KEY_H; break;
        case GLFW_KEY_I: event.KeyEvent.KeyCode = KEY_I; break;
        case GLFW_KEY_J: event.KeyEvent.KeyCode = KEY_J; break;
        case GLFW_KEY_K: event.KeyEvent.KeyCode = KEY_K; break;
        case GLFW_KEY_L: event.KeyEvent.KeyCode = KEY_L; break;
        case GLFW_KEY_M: event.KeyEvent.KeyCode = KEY_M; break;
        case GLFW_KEY_N: event.KeyEvent.KeyCode = KEY_N; break;
        case GLFW_KEY_O: event.KeyEvent.KeyCode = KEY_O; break;
        case GLFW_KEY_P: event.KeyEvent.KeyCode = KEY_P; break;
        case GLFW_KEY_Q: event.KeyEvent.KeyCode = KEY_Q; break;
        case GLFW_KEY_R: event.KeyEvent.KeyCode = KEY_R; break;
        case GLFW_KEY_S: event.KeyEvent.KeyCode = KEY_S; break;
        case GLFW_KEY_T: event.KeyEvent.KeyCode = KEY_T; break;
        case GLFW_KEY_U: event.KeyEvent.KeyCode = KEY_U; break;
        case GLFW_KEY_V: event.KeyEvent.KeyCode = KEY_V; break;
        case GLFW_KEY_W: event.KeyEvent.KeyCode = KEY_W; break;
        case GLFW_KEY_X: event.KeyEvent.KeyCode = KEY_X; break;
        case GLFW_KEY_Y: event.KeyEvent.KeyCode = KEY_Y; break;
        case GLFW_KEY_Z: event.KeyEvent.KeyCode = KEY_Z; break;
        case GLFW_KEY_LEFT_BRACKET: event.KeyEvent.KeyCode = KEY_LEFT_BRACKET; break;
        case GLFW_KEY_BACKSLASH: event.KeyEvent.KeyCode = KEY_BACKSLASH; break;
        case GLFW_KEY_RIGHT_BRACKET: event.KeyEvent.KeyCode = KEY_RIGHT_BRACKET; break;
        case GLFW_KEY_GRAVE_ACCENT: event.KeyEvent.KeyCode = KEY_GRAVE_ACCENT; break;
        case GLFW_KEY_WORLD_1: event.KeyEvent.KeyCode = KEY_WORLD_1; break;
        case GLFW_KEY_WORLD_2: event.KeyEvent.KeyCode = KEY_WORLD_2; break;
        case GLFW_KEY_ESCAPE: event.KeyEvent.KeyCode = KEY_ESCAPE; break;
        case GLFW_KEY_ENTER: event.KeyEvent.KeyCode = KEY_ENTER; break;
        case GLFW_KEY_TAB: event.KeyEvent.KeyCode = KEY_TAB; break;
        case GLFW_KEY_BACKSPACE: event.KeyEvent.KeyCode = KEY_BACKSPACE; break;
        case GLFW_KEY_INSERT: event.KeyEvent.KeyCode = KEY_INSERT; break;
        case GLFW_KEY_DELETE: event.KeyEvent.KeyCode = KEY_DELETE; break;
        case GLFW_KEY_RIGHT: event.KeyEvent.KeyCode = KEY_RIGHT; break;
        case GLFW_KEY_LEFT: event.KeyEvent.KeyCode = KEY_LEFT; break;
        case GLFW_KEY_DOWN: event.KeyEvent.KeyCode = KEY_DOWN; break;
        case GLFW_KEY_UP: event.KeyEvent.KeyCode = KEY_UP; break;
        case GLFW_KEY_PAGE_UP: event.KeyEvent.KeyCode = KEY_PAGE_UP; break;
        case GLFW_KEY_PAGE_DOWN: event.KeyEvent.KeyCode = KEY_PAGE_DOWN; break;
        case GLFW_KEY_HOME: event.KeyEvent.KeyCode = KEY_HOME; break;
        case GLFW_KEY_END: event.KeyEvent.KeyCode = KEY_END; break;
        case GLFW_KEY_CAPS_LOCK: event.KeyEvent.KeyCode = KEY_CAPS_LOCK; break;
        case GLFW_KEY_SCROLL_LOCK: event.KeyEvent.KeyCode = KEY_SCROLL_LOCK; break;
        case GLFW_KEY_NUM_LOCK: event.KeyEvent.KeyCode = KEY_NUM_LOCK; break;
        case GLFW_KEY_PRINT_SCREEN: event.KeyEvent.KeyCode = KEY_PRINT_SCREEN; break;
        case GLFW_KEY_PAUSE: event.KeyEvent.KeyCode = KEY_PAUSE; break;
        case GLFW_KEY_F1: event.KeyEvent.KeyCode = KEY_F1; break;
        case GLFW_KEY_F2: event.KeyEvent.KeyCode = KEY_F2; break;
        case GLFW_KEY_F3: event.KeyEvent.KeyCode = KEY_F3; break;
        case GLFW_KEY_F4: event.KeyEvent.KeyCode = KEY_F4; break;
        case GLFW_KEY_F5: event.KeyEvent.KeyCode = KEY_F5; break;
        case GLFW_KEY_F6: event.KeyEvent.KeyCode = KEY_F6; break;
        case GLFW_KEY_F7: event.KeyEvent.KeyCode = KEY_F7; break;
        case GLFW_KEY_F8: event.KeyEvent.KeyCode = KEY_F8; break;
        case GLFW_KEY_F9: event.KeyEvent.KeyCode = KEY_F9; break;
        case GLFW_KEY_F10: event.KeyEvent.KeyCode = KEY_F10; break;
        case GLFW_KEY_F11: event.KeyEvent.KeyCode = KEY_F11; break;
        case GLFW_KEY_F12: event.KeyEvent.KeyCode = KEY_F12; break;
        case GLFW_KEY_F13: event.KeyEvent.KeyCode = KEY_F13; break;
        case GLFW_KEY_F14: event.KeyEvent.KeyCode = KEY_F14; break;
        case GLFW_KEY_F15: event.KeyEvent.KeyCode = KEY_F15; break;
        case GLFW_KEY_F16: event.KeyEvent.KeyCode = KEY_F16; break;
        case GLFW_KEY_F17: event.KeyEvent.KeyCode = KEY_F17; break;
        case GLFW_KEY_F18: event.KeyEvent.KeyCode = KEY_F18; break;
        case GLFW_KEY_F19: event.KeyEvent.KeyCode = KEY_F19; break;
        case GLFW_KEY_F20: event.KeyEvent.KeyCode = KEY_F20; break;
        case GLFW_KEY_F21: event.KeyEvent.KeyCode = KEY_F21; break;
        case GLFW_KEY_F22: event.KeyEvent.KeyCode = KEY_F22; break;
        case GLFW_KEY_F23: event.KeyEvent.KeyCode = KEY_F23; break;
        case GLFW_KEY_F24: event.KeyEvent.KeyCode = KEY_F24; break;
        case GLFW_KEY_F25: event.KeyEvent.KeyCode = KEY_F25; break;
        case GLFW_KEY_KP_0: event.KeyEvent.KeyCode = KEY_KEYPAD_0; break;
        case GLFW_KEY_KP_1: event.KeyEvent.KeyCode = KEY_KEYPAD_1; break;
        case GLFW_KEY_KP_2: event.KeyEvent.KeyCode = KEY_KEYPAD_2; break;
        case GLFW_KEY_KP_3: event.KeyEvent.KeyCode = KEY_KEYPAD_3; break;
        case GLFW_KEY_KP_4: event.KeyEvent.KeyCode = KEY_KEYPAD_4; break;
        case GLFW_KEY_KP_5: event.KeyEvent.KeyCode = KEY_KEYPAD_5; break;
        case GLFW_KEY_KP_6: event.KeyEvent.KeyCode = KEY_KEYPAD_6; break;
        case GLFW_KEY_KP_7: event.KeyEvent.KeyCode = KEY_KEYPAD_7; break;
        case GLFW_KEY_KP_8: event.KeyEvent.KeyCode = KEY_KEYPAD_8; break;
        case GLFW_KEY_KP_9: event.KeyEvent.KeyCode = KEY_KEYPAD_9; break;
        case GLFW_KEY_KP_DECIMAL: event.KeyEvent.KeyCode = KEY_KEYPAD_DECIMAL; break;
        case GLFW_KEY_KP_DIVIDE: event.KeyEvent.KeyCode = KEY_KEYPAD_DIVIDE; break;
        case GLFW_KEY_KP_MULTIPLY: event.KeyEvent.KeyCode = KEY_KEYPAD_MULTIPLY; break;
        case GLFW_KEY_KP_SUBTRACT: event.KeyEvent.KeyCode = KEY_KEYPAD_SUBTRACT; break;
        case GLFW_KEY_KP_ADD: event.KeyEvent.KeyCode = KEY_KEYPAD_ADD; break;
        case GLFW_KEY_KP_ENTER: event.KeyEvent.KeyCode = KEY_KEYPAD_ENTER; break;
        case GLFW_KEY_KP_EQUAL: event.KeyEvent.KeyCode = KEY_KEYPAD_EQUAL; break;
        case GLFW_KEY_LEFT_SHIFT: event.KeyEvent.KeyCode = KEY_LEFT_SHIFT; break;
        case GLFW_KEY_LEFT_CONTROL: event.KeyEvent.KeyCode = KEY_LEFT_CONTROL; break;
        case GLFW_KEY_LEFT_ALT: event.KeyEvent.KeyCode = KEY_LEFT_ALT; break;
        case GLFW_KEY_LEFT_SUPER: event.KeyEvent.KeyCode = KEY_LEFT_SUPER; break;
        case GLFW_KEY_RIGHT_SHIFT: event.KeyEvent.KeyCode = KEY_RIGHT_SHIFT; break;
        case GLFW_KEY_RIGHT_CONTROL: event.KeyEvent.KeyCode = KEY_RIGHT_CONTROL; break;
        case GLFW_KEY_RIGHT_ALT: event.KeyEvent.KeyCode = KEY_RIGHT_ALT; break;
        case GLFW_KEY_RIGHT_SUPER: event.KeyEvent.KeyCode = KEY_RIGHT_SUPER; break;
        case GLFW_KEY_MENU: event.KeyEvent.KeyCode = KEY_MENU; break;
        default: event.KeyEvent.KeyCode = KEY_UNKNOWN; break;
        }
        
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
}
#endif
