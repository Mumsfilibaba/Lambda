#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_MACOS)
    #include "macOSInput.h"
    #include "System/Application.h"

namespace Lambda
{
    //-----
    //Input
    //-----
    
    Input* Input::Create()
    {
        return DBG_NEW MacOSInput();
    }
    
    //----------
    //MacOSInput
    //----------

    MacOSInput::MacOSInput()
        : m_LambdaKeyTable(),
        m_GLFWKeyTable()
    {
        //Lambda key table
        memset(m_LambdaKeyTable, KEY_UNKNOWN, sizeof(m_LambdaKeyTable));
        m_LambdaKeyTable[GLFW_KEY_SPACE]            = KEY_SPACE;
        m_LambdaKeyTable[GLFW_KEY_APOSTROPHE]       = KEY_APOSTROPHE;
        m_LambdaKeyTable[GLFW_KEY_COMMA]            = KEY_COMMA;
        m_LambdaKeyTable[GLFW_KEY_MINUS]            = KEY_MINUS;
        m_LambdaKeyTable[GLFW_KEY_PERIOD]           = KEY_PERIOD;
        m_LambdaKeyTable[GLFW_KEY_SLASH]            = KEY_SLASH;
        m_LambdaKeyTable[GLFW_KEY_0]                = KEY_0;
        m_LambdaKeyTable[GLFW_KEY_1]                = KEY_1;
        m_LambdaKeyTable[GLFW_KEY_2]                = KEY_2;
        m_LambdaKeyTable[GLFW_KEY_3]                = KEY_3;
        m_LambdaKeyTable[GLFW_KEY_4]                = KEY_4;
        m_LambdaKeyTable[GLFW_KEY_5]                = KEY_5;
        m_LambdaKeyTable[GLFW_KEY_6]                = KEY_6;
        m_LambdaKeyTable[GLFW_KEY_7]                = KEY_7;
        m_LambdaKeyTable[GLFW_KEY_8]                = KEY_8;
        m_LambdaKeyTable[GLFW_KEY_9]                = KEY_9;
        m_LambdaKeyTable[GLFW_KEY_SEMICOLON]        = KEY_SEMICOLON;
        m_LambdaKeyTable[GLFW_KEY_EQUAL]            = KEY_EQUAL;
        m_LambdaKeyTable[GLFW_KEY_A]                = KEY_A;
        m_LambdaKeyTable[GLFW_KEY_B]                = KEY_B;
        m_LambdaKeyTable[GLFW_KEY_C]                = KEY_C;
        m_LambdaKeyTable[GLFW_KEY_D]                = KEY_D;
        m_LambdaKeyTable[GLFW_KEY_E]                = KEY_E;
        m_LambdaKeyTable[GLFW_KEY_F]                = KEY_F;
        m_LambdaKeyTable[GLFW_KEY_G]                = KEY_G;
        m_LambdaKeyTable[GLFW_KEY_H]                = KEY_H;
        m_LambdaKeyTable[GLFW_KEY_I]                = KEY_I;
        m_LambdaKeyTable[GLFW_KEY_J]                = KEY_J;
        m_LambdaKeyTable[GLFW_KEY_K]                = KEY_K;
        m_LambdaKeyTable[GLFW_KEY_L]                = KEY_L;
        m_LambdaKeyTable[GLFW_KEY_M]                = KEY_M;
        m_LambdaKeyTable[GLFW_KEY_N]                = KEY_N;
        m_LambdaKeyTable[GLFW_KEY_O]                = KEY_O;
        m_LambdaKeyTable[GLFW_KEY_P]                = KEY_P;
        m_LambdaKeyTable[GLFW_KEY_Q]                = KEY_Q;
        m_LambdaKeyTable[GLFW_KEY_R]                = KEY_R;
        m_LambdaKeyTable[GLFW_KEY_S]                = KEY_S;
        m_LambdaKeyTable[GLFW_KEY_T]                = KEY_T;
        m_LambdaKeyTable[GLFW_KEY_U]                = KEY_U;
        m_LambdaKeyTable[GLFW_KEY_V]                = KEY_V;
        m_LambdaKeyTable[GLFW_KEY_W]                = KEY_W;
        m_LambdaKeyTable[GLFW_KEY_X]                = KEY_X;
        m_LambdaKeyTable[GLFW_KEY_Y]                = KEY_Y;
        m_LambdaKeyTable[GLFW_KEY_Z]                = KEY_Z;
        m_LambdaKeyTable[GLFW_KEY_LEFT_BRACKET]     = KEY_LEFT_BRACKET;
        m_LambdaKeyTable[GLFW_KEY_BACKSLASH]        = KEY_BACKSLASH;
        m_LambdaKeyTable[GLFW_KEY_RIGHT_BRACKET]    = KEY_RIGHT_BRACKET;
        m_LambdaKeyTable[GLFW_KEY_GRAVE_ACCENT]     = KEY_GRAVE_ACCENT;
        m_LambdaKeyTable[GLFW_KEY_WORLD_1]          = KEY_WORLD_1;
        m_LambdaKeyTable[GLFW_KEY_WORLD_2]          = KEY_WORLD_2;
        m_LambdaKeyTable[GLFW_KEY_ESCAPE]           = KEY_ESCAPE;
        m_LambdaKeyTable[GLFW_KEY_ENTER]            = KEY_ENTER;
        m_LambdaKeyTable[GLFW_KEY_TAB]              = KEY_TAB;
        m_LambdaKeyTable[GLFW_KEY_BACKSPACE]        = KEY_BACKSPACE;
        m_LambdaKeyTable[GLFW_KEY_INSERT]           = KEY_INSERT;
        m_LambdaKeyTable[GLFW_KEY_DELETE]           = KEY_DELETE;
        m_LambdaKeyTable[GLFW_KEY_RIGHT]            = KEY_RIGHT;
        m_LambdaKeyTable[GLFW_KEY_LEFT]             = KEY_LEFT;
        m_LambdaKeyTable[GLFW_KEY_DOWN]             = KEY_DOWN;
        m_LambdaKeyTable[GLFW_KEY_UP]               = KEY_UP;
        m_LambdaKeyTable[GLFW_KEY_PAGE_UP]          = KEY_PAGE_UP;
        m_LambdaKeyTable[GLFW_KEY_PAGE_DOWN]        = KEY_PAGE_DOWN;
        m_LambdaKeyTable[GLFW_KEY_HOME]             = KEY_HOME;
        m_LambdaKeyTable[GLFW_KEY_END]              = KEY_END;
        m_LambdaKeyTable[GLFW_KEY_CAPS_LOCK]        = KEY_CAPS_LOCK;
        m_LambdaKeyTable[GLFW_KEY_SCROLL_LOCK]      = KEY_SCROLL_LOCK;
        m_LambdaKeyTable[GLFW_KEY_NUM_LOCK]         = KEY_NUM_LOCK;
        m_LambdaKeyTable[GLFW_KEY_PRINT_SCREEN]     = KEY_PRINT_SCREEN;
        m_LambdaKeyTable[GLFW_KEY_PAUSE]            = KEY_PAUSE;
        m_LambdaKeyTable[GLFW_KEY_F1]               = KEY_F1;
        m_LambdaKeyTable[GLFW_KEY_F2]               = KEY_F2;
        m_LambdaKeyTable[GLFW_KEY_F3]               = KEY_F3;
        m_LambdaKeyTable[GLFW_KEY_F4]               = KEY_F4;
        m_LambdaKeyTable[GLFW_KEY_F5]               = KEY_F5;
        m_LambdaKeyTable[GLFW_KEY_F6]               = KEY_F6;
        m_LambdaKeyTable[GLFW_KEY_F7]               = KEY_F7;
        m_LambdaKeyTable[GLFW_KEY_F8]               = KEY_F8;
        m_LambdaKeyTable[GLFW_KEY_F9]               = KEY_F9;
        m_LambdaKeyTable[GLFW_KEY_F10]              = KEY_F10;
        m_LambdaKeyTable[GLFW_KEY_F11]              = KEY_F11;
        m_LambdaKeyTable[GLFW_KEY_F12]              = KEY_F12;
        m_LambdaKeyTable[GLFW_KEY_F13]              = KEY_F13;
        m_LambdaKeyTable[GLFW_KEY_F14]              = KEY_F14;
        m_LambdaKeyTable[GLFW_KEY_F15]              = KEY_F15;
        m_LambdaKeyTable[GLFW_KEY_F16]              = KEY_F16;
        m_LambdaKeyTable[GLFW_KEY_F17]              = KEY_F17;
        m_LambdaKeyTable[GLFW_KEY_F18]              = KEY_F18;
        m_LambdaKeyTable[GLFW_KEY_F19]              = KEY_F19;
        m_LambdaKeyTable[GLFW_KEY_F20]              = KEY_F20;
        m_LambdaKeyTable[GLFW_KEY_F21]              = KEY_F21;
        m_LambdaKeyTable[GLFW_KEY_F22]              = KEY_F22;
        m_LambdaKeyTable[GLFW_KEY_F23]              = KEY_F23;
        m_LambdaKeyTable[GLFW_KEY_F24]              = KEY_F24;
        m_LambdaKeyTable[GLFW_KEY_F25]              = KEY_F25;
        m_LambdaKeyTable[GLFW_KEY_KP_0]             = KEY_KEYPAD_0;
        m_LambdaKeyTable[GLFW_KEY_KP_1]             = KEY_KEYPAD_1;
        m_LambdaKeyTable[GLFW_KEY_KP_2]             = KEY_KEYPAD_2;
        m_LambdaKeyTable[GLFW_KEY_KP_3]             = KEY_KEYPAD_3;
        m_LambdaKeyTable[GLFW_KEY_KP_4]             = KEY_KEYPAD_4;
        m_LambdaKeyTable[GLFW_KEY_KP_5]             = KEY_KEYPAD_5;
        m_LambdaKeyTable[GLFW_KEY_KP_6]             = KEY_KEYPAD_6;
        m_LambdaKeyTable[GLFW_KEY_KP_7]             = KEY_KEYPAD_7;
        m_LambdaKeyTable[GLFW_KEY_KP_8]             = KEY_KEYPAD_8;
        m_LambdaKeyTable[GLFW_KEY_KP_9]             = KEY_KEYPAD_9;
        m_LambdaKeyTable[GLFW_KEY_KP_DECIMAL]       = KEY_KEYPAD_DECIMAL;
        m_LambdaKeyTable[GLFW_KEY_KP_DIVIDE]        = KEY_KEYPAD_DIVIDE;
        m_LambdaKeyTable[GLFW_KEY_KP_MULTIPLY]      = KEY_KEYPAD_MULTIPLY;
        m_LambdaKeyTable[GLFW_KEY_KP_SUBTRACT]      = KEY_KEYPAD_SUBTRACT;
        m_LambdaKeyTable[GLFW_KEY_KP_ADD]           = KEY_KEYPAD_ADD;
        m_LambdaKeyTable[GLFW_KEY_KP_ENTER]         = KEY_KEYPAD_ENTER;
        m_LambdaKeyTable[GLFW_KEY_KP_EQUAL]         = KEY_KEYPAD_EQUAL;
        m_LambdaKeyTable[GLFW_KEY_LEFT_SHIFT]       = KEY_LEFT_SHIFT;
        m_LambdaKeyTable[GLFW_KEY_LEFT_CONTROL]     = KEY_LEFT_CONTROL;
        m_LambdaKeyTable[GLFW_KEY_LEFT_ALT]         = KEY_LEFT_ALT;
        m_LambdaKeyTable[GLFW_KEY_LEFT_SUPER]       = KEY_LEFT_SUPER;
        m_LambdaKeyTable[GLFW_KEY_RIGHT_SHIFT]      = KEY_RIGHT_SHIFT;
        m_LambdaKeyTable[GLFW_KEY_RIGHT_CONTROL]    = KEY_RIGHT_CONTROL;
        m_LambdaKeyTable[GLFW_KEY_RIGHT_ALT]        = KEY_RIGHT_ALT;
        m_LambdaKeyTable[GLFW_KEY_RIGHT_SUPER]      = KEY_RIGHT_SUPER;
        m_LambdaKeyTable[GLFW_KEY_MENU]             = KEY_MENU;
        
        //GLFW table
        memset(m_GLFWKeyTable, 0, sizeof(m_GLFWKeyTable));
        m_GLFWKeyTable[KEY_SPACE]            = GLFW_KEY_SPACE;
        m_GLFWKeyTable[KEY_APOSTROPHE]       = GLFW_KEY_APOSTROPHE;
        m_GLFWKeyTable[KEY_COMMA]            = GLFW_KEY_COMMA;
        m_GLFWKeyTable[KEY_MINUS]            = GLFW_KEY_MINUS;
        m_GLFWKeyTable[KEY_PERIOD]           = GLFW_KEY_PERIOD;
        m_GLFWKeyTable[KEY_SLASH]            = GLFW_KEY_SLASH;
        m_GLFWKeyTable[KEY_0]                = GLFW_KEY_0;
        m_GLFWKeyTable[KEY_1]                = GLFW_KEY_1;
        m_GLFWKeyTable[KEY_2]                = GLFW_KEY_2;
        m_GLFWKeyTable[KEY_3]                = GLFW_KEY_3;
        m_GLFWKeyTable[KEY_4]                = GLFW_KEY_4;
        m_GLFWKeyTable[KEY_5]                = GLFW_KEY_5;
        m_GLFWKeyTable[KEY_6]                = GLFW_KEY_6;
        m_GLFWKeyTable[KEY_7]                = GLFW_KEY_7;
        m_GLFWKeyTable[KEY_8]                = GLFW_KEY_8;
        m_GLFWKeyTable[KEY_9]                = GLFW_KEY_9;
        m_GLFWKeyTable[KEY_SEMICOLON]        = GLFW_KEY_SEMICOLON;
        m_GLFWKeyTable[KEY_EQUAL]            = GLFW_KEY_EQUAL;
        m_GLFWKeyTable[KEY_A]                = GLFW_KEY_A;
        m_GLFWKeyTable[KEY_B]                = GLFW_KEY_B;
        m_GLFWKeyTable[KEY_C]                = GLFW_KEY_C;
        m_GLFWKeyTable[KEY_D]                = GLFW_KEY_D;
        m_GLFWKeyTable[KEY_E]                = GLFW_KEY_E;
        m_GLFWKeyTable[KEY_F]                = GLFW_KEY_F;
        m_GLFWKeyTable[KEY_G]                = GLFW_KEY_G;
        m_GLFWKeyTable[KEY_H]                = GLFW_KEY_H;
        m_GLFWKeyTable[KEY_I]                = GLFW_KEY_I;
        m_GLFWKeyTable[KEY_J]                = GLFW_KEY_J;
        m_GLFWKeyTable[KEY_K]                = GLFW_KEY_K;
        m_GLFWKeyTable[KEY_L]                = GLFW_KEY_L;
        m_GLFWKeyTable[KEY_M]                = GLFW_KEY_M;
        m_GLFWKeyTable[KEY_N]                = GLFW_KEY_N;
        m_GLFWKeyTable[KEY_O]                = GLFW_KEY_O;
        m_GLFWKeyTable[KEY_P]                = GLFW_KEY_P;
        m_GLFWKeyTable[KEY_Q]                = GLFW_KEY_Q;
        m_GLFWKeyTable[KEY_R]                = GLFW_KEY_R;
        m_GLFWKeyTable[KEY_S]                = GLFW_KEY_S;
        m_GLFWKeyTable[KEY_T]                = GLFW_KEY_T;
        m_GLFWKeyTable[KEY_U]                = GLFW_KEY_U;
        m_GLFWKeyTable[KEY_V]                = GLFW_KEY_V;
        m_GLFWKeyTable[KEY_W]                = GLFW_KEY_W;
        m_GLFWKeyTable[KEY_X]                = GLFW_KEY_X;
        m_GLFWKeyTable[KEY_Y]                = GLFW_KEY_Y;
        m_GLFWKeyTable[KEY_Z]                = GLFW_KEY_Z;
        m_GLFWKeyTable[KEY_LEFT_BRACKET]     = GLFW_KEY_LEFT_BRACKET;
        m_GLFWKeyTable[KEY_BACKSLASH]        = GLFW_KEY_BACKSLASH;
        m_GLFWKeyTable[KEY_RIGHT_BRACKET]    = GLFW_KEY_RIGHT_BRACKET;
        m_GLFWKeyTable[KEY_GRAVE_ACCENT]     = GLFW_KEY_GRAVE_ACCENT;
        m_GLFWKeyTable[KEY_WORLD_1]          = GLFW_KEY_WORLD_1;
        m_GLFWKeyTable[KEY_WORLD_2]          = GLFW_KEY_WORLD_2;
        m_GLFWKeyTable[KEY_ESCAPE]           = GLFW_KEY_ESCAPE;
        m_GLFWKeyTable[KEY_ENTER]            = GLFW_KEY_ENTER;
        m_GLFWKeyTable[KEY_TAB]              = GLFW_KEY_TAB;
        m_GLFWKeyTable[KEY_BACKSPACE]        = GLFW_KEY_BACKSPACE;
        m_GLFWKeyTable[KEY_INSERT]           = GLFW_KEY_INSERT;
        m_GLFWKeyTable[KEY_DELETE]           = GLFW_KEY_DELETE;
        m_GLFWKeyTable[KEY_RIGHT]            = GLFW_KEY_RIGHT;
        m_GLFWKeyTable[KEY_LEFT]             = GLFW_KEY_LEFT;
        m_GLFWKeyTable[KEY_DOWN]             = GLFW_KEY_DOWN;
        m_GLFWKeyTable[KEY_UP]               = GLFW_KEY_UP;
        m_GLFWKeyTable[KEY_PAGE_UP]          = GLFW_KEY_PAGE_UP;
        m_GLFWKeyTable[KEY_PAGE_DOWN]        = GLFW_KEY_PAGE_DOWN;
        m_GLFWKeyTable[KEY_HOME]             = GLFW_KEY_HOME;
        m_GLFWKeyTable[KEY_END]              = GLFW_KEY_END;
        m_GLFWKeyTable[KEY_CAPS_LOCK]        = GLFW_KEY_CAPS_LOCK;
        m_GLFWKeyTable[KEY_SCROLL_LOCK]      = GLFW_KEY_SCROLL_LOCK;
        m_GLFWKeyTable[KEY_NUM_LOCK]         = GLFW_KEY_NUM_LOCK;
        m_GLFWKeyTable[KEY_PRINT_SCREEN]     = GLFW_KEY_PRINT_SCREEN;
        m_GLFWKeyTable[KEY_PAUSE]            = GLFW_KEY_PAUSE;
        m_GLFWKeyTable[KEY_F1]               = GLFW_KEY_F1;
        m_GLFWKeyTable[KEY_F2]               = GLFW_KEY_F2;
        m_GLFWKeyTable[KEY_F3]               = GLFW_KEY_F3;
        m_GLFWKeyTable[KEY_F4]               = GLFW_KEY_F4;
        m_GLFWKeyTable[KEY_F5]               = GLFW_KEY_F5;
        m_GLFWKeyTable[KEY_F6]               = GLFW_KEY_F6;
        m_GLFWKeyTable[KEY_F7]               = GLFW_KEY_F7;
        m_GLFWKeyTable[KEY_F8]               = GLFW_KEY_F8;
        m_GLFWKeyTable[KEY_F9]               = GLFW_KEY_F9;
        m_GLFWKeyTable[KEY_F10]              = GLFW_KEY_F10;
        m_GLFWKeyTable[KEY_F11]              = GLFW_KEY_F11;
        m_GLFWKeyTable[KEY_F12]              = GLFW_KEY_F12;
        m_GLFWKeyTable[KEY_F13]              = GLFW_KEY_F13;
        m_GLFWKeyTable[KEY_F14]              = GLFW_KEY_F14;
        m_GLFWKeyTable[KEY_F15]              = GLFW_KEY_F15;
        m_GLFWKeyTable[KEY_F16]              = GLFW_KEY_F16;
        m_GLFWKeyTable[KEY_F17]              = GLFW_KEY_F17;
        m_GLFWKeyTable[KEY_F18]              = GLFW_KEY_F18;
        m_GLFWKeyTable[KEY_F19]              = GLFW_KEY_F19;
        m_GLFWKeyTable[KEY_F20]              = GLFW_KEY_F20;
        m_GLFWKeyTable[KEY_F21]              = GLFW_KEY_F21;
        m_GLFWKeyTable[KEY_F22]              = GLFW_KEY_F22;
        m_GLFWKeyTable[KEY_F23]              = GLFW_KEY_F23;
        m_GLFWKeyTable[KEY_F24]              = GLFW_KEY_F24;
        m_GLFWKeyTable[KEY_F25]              = GLFW_KEY_F25;
        m_GLFWKeyTable[KEY_KEYPAD_0]         = GLFW_KEY_KP_0;
        m_GLFWKeyTable[KEY_KEYPAD_1]         = GLFW_KEY_KP_1;
        m_GLFWKeyTable[KEY_KEYPAD_2]         = GLFW_KEY_KP_2;
        m_GLFWKeyTable[KEY_KEYPAD_3]         = GLFW_KEY_KP_3;
        m_GLFWKeyTable[KEY_KEYPAD_4]         = GLFW_KEY_KP_4;
        m_GLFWKeyTable[KEY_KEYPAD_5]         = GLFW_KEY_KP_5;
        m_GLFWKeyTable[KEY_KEYPAD_6]         = GLFW_KEY_KP_6;
        m_GLFWKeyTable[KEY_KEYPAD_7]         = GLFW_KEY_KP_7;
        m_GLFWKeyTable[KEY_KEYPAD_8]         = GLFW_KEY_KP_8;
        m_GLFWKeyTable[KEY_KEYPAD_9]         = GLFW_KEY_KP_9;
        m_GLFWKeyTable[KEY_KEYPAD_DECIMAL]   = GLFW_KEY_KP_DECIMAL;
        m_GLFWKeyTable[KEY_KEYPAD_DIVIDE]    = GLFW_KEY_KP_DIVIDE;
        m_GLFWKeyTable[KEY_KEYPAD_MULTIPLY]  = GLFW_KEY_KP_MULTIPLY;
        m_GLFWKeyTable[KEY_KEYPAD_SUBTRACT]  = GLFW_KEY_KP_SUBTRACT;
        m_GLFWKeyTable[KEY_KEYPAD_ADD]       = GLFW_KEY_KP_ADD;
        m_GLFWKeyTable[KEY_KEYPAD_ENTER]     = GLFW_KEY_KP_ENTER;
        m_GLFWKeyTable[KEY_KEYPAD_EQUAL]     = GLFW_KEY_KP_EQUAL;
        m_GLFWKeyTable[KEY_LEFT_SHIFT]       = GLFW_KEY_LEFT_SHIFT;
        m_GLFWKeyTable[KEY_LEFT_CONTROL]     = GLFW_KEY_LEFT_CONTROL;
        m_GLFWKeyTable[KEY_LEFT_ALT]         = GLFW_KEY_LEFT_ALT;
        m_GLFWKeyTable[KEY_LEFT_SUPER]       = GLFW_KEY_LEFT_SUPER;
        m_GLFWKeyTable[KEY_RIGHT_SHIFT]      = GLFW_KEY_RIGHT_SHIFT;
        m_GLFWKeyTable[KEY_RIGHT_CONTROL]    = GLFW_KEY_RIGHT_CONTROL;
        m_GLFWKeyTable[KEY_RIGHT_ALT]        = GLFW_KEY_RIGHT_ALT;
        m_GLFWKeyTable[KEY_RIGHT_SUPER]      = GLFW_KEY_RIGHT_SUPER;
        m_GLFWKeyTable[KEY_MENU]             = GLFW_KEY_MENU;
    }
    
    
    bool MacOSInput::InternalIsKeyUp(Key keycode) const
    {
        GLFWwindow* pWindow = reinterpret_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeHandle());
        return glfwGetKey(pWindow, LambdaKeyToGLFW(keycode)) == GLFW_RELEASE;
    }
    
    
    bool MacOSInput::InternalIsKeyDown(Key keycode) const
    {
        GLFWwindow* pWindow = reinterpret_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeHandle());
        return glfwGetKey(pWindow, LambdaKeyToGLFW(keycode)) == GLFW_PRESS;
    }
    
    
    void MacOSInput::InternalSetMousePosition(float x, float y) const
    {
        GLFWwindow* pWindow = reinterpret_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeHandle());
        glfwSetCursorPos(pWindow, double(x), double(y));
    }
    
    
    void MacOSInput::InternalGetMousePosition(float& x, float& y) const
    {
        GLFWwindow* pWindow = reinterpret_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeHandle());
        double dx = 0.0;
        double dy = 0.0;
        glfwGetCursorPos(pWindow, &dx, &dy);
        
        x = float(dx);
        y = float(dy);
    }
    
    
    void MacOSInput::InternalSetMousePosition(IWindow* pRelativeTo, float x, float y) const
    {
        GLFWwindow* pWindow = reinterpret_cast<GLFWwindow*>(pRelativeTo->GetNativeHandle());
        glfwSetCursorPos(pWindow, double(x), double(y));
    }
    
    
    void MacOSInput::InternalGetMousePosition(IWindow* pRelativeTo, float& x, float& y) const
    {
        GLFWwindow* pWindow = reinterpret_cast<GLFWwindow*>(pRelativeTo->GetNativeHandle());
        double dx = 0.0;
        double dy = 0.0;
        glfwGetCursorPos(pWindow, &dx, &dy);
        
        x = float(dx);
        y = float(dy);
    }
    
    
    Key MacOSInput::GLFWKeyToLambda(uint32 keycode) const
    {
        return m_LambdaKeyTable[keycode];
    }
    
    
    uint32 MacOSInput::LambdaKeyToGLFW(Key keycode) const
    {
        return m_GLFWKeyTable[keycode];
    }
}

#endif
