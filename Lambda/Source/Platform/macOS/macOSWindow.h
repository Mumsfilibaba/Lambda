#pragma once
#include "System/IWindow.h"
#if defined(LAMBDA_PLAT_MACOS)
#include <GLFW/glfw3.h>

namespace Lambda
{
    class MacOSWindow : public IWindow
    {
    public:
        MacOSWindow(const WindowDesc& desc);
        ~MacOSWindow();
        
        virtual void SetEventCallback(EventCallback callback) override final;
        virtual void OnUpdate() const override final;
        
        virtual uint32 GetHeight() const override final;
        virtual uint32 GetWidth() const override final;
        virtual void* GetNativeHandle() const override final;
        
    private:
        void Init(const WindowDesc& desc);
        
    private:
        GLFWwindow* m_pWindow;
        uint32 m_Width;
        uint32 m_Height;
        EventCallback m_OnEvent;
        
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
        static bool s_HasInitGLFW;
    };
}
#endif