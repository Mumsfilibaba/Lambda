#pragma once
#include "System/IWindow.h"
#if defined(LAMBDA_PLAT_MACOS)
namespace Lambda
{
    class MacOSWindow : public IWindow
    {
    public:
        MacOSWindow();
        ~MacOSWindow();
        
        virtual void SetEventCallback(EventCallback callback) override final;
        virtual void OnUpdate() const override final;
        
        virtual uint32 GetHeight() const override final;
        virtual uint32 GetWidth() const override final;
        virtual void* GetNativeHandle() const override final;
    private:
    };
}
#endif
