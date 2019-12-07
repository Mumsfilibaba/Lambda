#pragma once
#if defined(LAMBDA_PLAT_MACOS)
    #include "IObject.h"
    #include <GLFW/glfw3.h>

namespace Lambda
{
    //-----------
    //macOSWindow
    //-----------
    
    class macOSWindow
    {
    public:
        macOSWindow(const WindowDesc& desc);
        ~macOSWindow();
        
        virtual void OnUpdate() const override final;
        virtual bool HasFocus() const override final;
        
        virtual void SetEventCallback(IEventCallback* pCallback) override final;
        virtual bool SetFullscreen(bool fullscreen) override final;
        virtual bool GetFullscreen() const override final;
        virtual uint32 GetHeight() const override final;
        virtual uint32 GetWidth() const override final;
        virtual void* GetNativeHandle() const override final;
    private:
        void Init(const WindowDesc& desc);
        void DispatchEvent(const Event& event);
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
