#pragma once
#include "System/IWindow.h"
#if defined(LAMBDA_PLAT_MACOS)
#include <GLFW/glfw3.h>

namespace Lambda
{
    //-----------
    //MacOSWindow
    //-----------
    
    class MacOSWindow : public IWindow
    {
    public:
        MacOSWindow(const WindowDesc& desc);
        ~MacOSWindow();
        
        virtual void SetEventCallback(EventCallback callback) override final;
        virtual void OnUpdate() const override final;
        virtual bool SetFullscreen(bool fullscreen) override final;
        virtual bool GetFullscreen() const override final;
        
        virtual IGraphicsDevice*    GetGraphicsDevice() const override final;
        virtual uint32              GetHeight() const override final;
        virtual uint32              GetWidth() const override final;
        virtual void*               GetNativeHandle() const override final;
        
        
    private:
        void Init(const WindowDesc& desc);
        void DispatchEvent(const Event& event);
        
    private:
        GLFWwindow*         m_pWindow;
        IGraphicsDevice*    m_pGraphicsDevice;
        std::vector<Event>  m_EventBackLog;
        uint32              m_Width;
        uint32              m_Height;
        EventCallback       m_EventCallback;
        bool                m_Fullscreen;
        
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
