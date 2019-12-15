#pragma once
#if defined(LAMBDA_PLAT_MACOS)
#include "../Desktop/CWindowBase.h"
#include <GLFW/glfw3.h>

namespace Lambda
{
    //------------
    //CmacOSWindow
    //------------
    
    class CmacOSWindow final : public CWindowBase
    {
    public:
        CmacOSWindow(const char* pTitle, uint32 width, uint32 height);
        ~CmacOSWindow();
        
        virtual bool HasFocus() const override final;
        virtual uint32 GetHeight() const override final;
        virtual uint32 GetWidth() const override final;
        
        bool SetFullscreen(bool fullscreen);
        bool GetFullscreen() const;
        void ProcessEvents() const;
    private:
        GLFWwindow* m_pWindow;
        IEventCallback* m_pCallback;
        uint32 m_Width;
        uint32 m_Height;
        bool m_Fullscreen;
        bool m_HasFocus;
    private:
        static void WindowClosedCallback(GLFWwindow* pWindow);
        static void WindowResizeCallback(GLFWwindow* pWindow, int32 width, int32 height);
        static void WindowMoveCallback(GLFWwindow* pWindow, int32 x, int32 y);
        static void KeyCallback(GLFWwindow* pWindow, int32 key, int32 scancode, int32 action, int32 mods);
        static void TextCallback(GLFWwindow* pWindow, uint32 codepoint);
        static void MouseMoveCallback(GLFWwindow* pWindow, double x, double y);
        static void MouseButtonCallback(GLFWwindow* pWindow, int32 button, int32 action, int32 mods);
        static void MouseScollCallback(GLFWwindow* pWindow, double xoffset, double yoffset);
        static void WindowFocusCallback(GLFWwindow* pWindow, int32 focused);
    private:
        static bool s_HasGLFW;
    };
}
#endif
