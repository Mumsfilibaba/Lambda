#pragma once
#include "LambdaCore.h"

#if defined(LAMBDA_PLAT_MACOS)
    #include "Core/Input/Mouse.h"
    #include "Core/Input/Keyboard.h"

    #include "Platform/Common/IWindow.h"
    #include "Platform/macOS/CocoaWindow.h"

    #include <vector>

namespace Lambda
{
    struct SEvent;

    class CMacWindow : public IWindow
    {
    public:
        CMacWindow(const SWindowDesc& desc);
        ~CMacWindow();
        
        /*IWindow*/
        virtual void Show() override final;
        
        virtual void AddEventListener(IEventListener*) override final;
        virtual void RemoveEventListener(IEventListener*) override final;
        
        virtual uint32 GetWidth() const override final { return m_nWidth; }
        virtual uint32 GetHeight() const override final { return m_nHeight; }
        virtual void*  GetNativeHandle() const override final { return m_pCocoaWindow; }
    public:
        void OnMove(uint32 x, uint32 y);
        void OnFocusChanged(bool bHasFocus);
        void OnResize(uint32 nWidth, uint32 nHeight);
        void OnMiniaturized(uint32 nWidth, uint32 nHeight, bool bMiniturized);
        void OnToggleFullscreen(bool bFullscreen);
        
        void OnKeyup(EKey keycode, uint32 modifiers);
        void OnKeydown(EKey keycode, uint32 modifiers);
        void OnKeyTyped(uint32 character);
        
        void OnMouseMove(uint32 x, uint32 y);
        void OnMouseButtonUp(EMouseButton button, uint32 modifiers);
        void OnMouseButtonDown(EMouseButton button, uint32 modifiers);
        void OnMouseScroll(float horizontalValue, float verticalValue);
        
        void OnClose();
    private:
        void Init(const SWindowDesc& desc);
        void DispatchEvent(const SEvent& event);
    private:
        std::vector<IEventListener*> m_EventListeners;
        CocoaView* m_pView;
        CocoaWindow* m_pCocoaWindow;
        uint32 m_Width;
        uint32 m_Height;
        bool m_bIsMiniaturized;
    };
}
#endif
