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
        
    private:
        static bool s_HasInitGLFW;
    };
}
#endif
