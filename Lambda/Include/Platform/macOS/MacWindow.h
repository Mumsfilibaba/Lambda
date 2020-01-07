#pragma once
#include "LambdaCore.h"

#if defined(LAMBDA_PLAT_MACOS)
    #include "Core/Input/EMouse.h"
    #include "Core/Input/EKeyboard.h"

    #include "Core/Engine/IWindow.h"
    
    #include "Platform/macOS/CocoaWindow.h"

namespace Lambda
{
    class CMacSystem;

    //----------
    //CMacWindow
    //----------
    class CMacWindow : public IWindow
    {
    public:
        CMacWindow(CMacSystem* pSystem, const char* pTitle, uint32 nWidth, uint32 nHeight);
        ~CMacWindow();
        
        /*IWindow Interface*/
        virtual bool HasFocus() const override final { return m_bHasFocus; }
        virtual bool IsMiniaturized() const override final { return m_bIsMiniaturized; }
        virtual bool IsFullscreen() const override final { return m_bIsFullscreen; };
        
        virtual void SetFullscreen(bool bFullscreen);
        
        virtual uint32 GetWidth() const override final { return m_nWidth; }
        virtual uint32 GetHeight() const override final { return m_nHeight; }
        virtual void*  GetNativeHandle() const override final { return m_pCocoaWindow; }
        
        /*CMacWindow Interface*/
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
        CMacSystem* m_pSystem;
        CCocoaView* m_pView;
        CCocoaWindow* m_pCocoaWindow;
        
        uint32 m_nWidth;
        uint32 m_nHeight;
        struct
        {
            uint32 x;
            uint32 y;
        } m_Position;
        
        bool m_bHasFocus;
        bool m_bIsFullscreen;
        bool m_bIsMiniaturized;
    };
}
#endif
