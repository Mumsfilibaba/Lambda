#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    //------------
    //EDisplayMode
    //------------

    enum class EDisplayMode
    {
        DISPLAY_MODE_WINDOWED              = 0,
        DISPLAY_MODE_FULLSCREEN            = 1,
        DISPLAY_MODE_BORDERLESS_FULLSCREEN = 2
    };

	//-------
	//CWindow
	//-------
	
    class CWindow
	{
    protected:
        LAMBDA_NO_COPY(CWindow);
        
        CWindow();
        ~CWindow();
    public:

        virtual void SetDisplayMode(EDisplayMode displayMode) {}
        virtual EDisplayMode GetDisplayMode() const { return EDisplayMode::DISPLAY_MODE_WINDOWED; };
        
        virtual void Show();
        virtual void Hide();
        virtual void Maximize();
        virtual void Minimize();
        
        virtual bool HasFocus() const { return false; }
        virtual bool IsFullscreen() const { return false; }
        virtual bool IsMaximized() const { return false; }
        virtual bool IsMiniaturized() const { return false; }
        
        virtual void SetFocus() {}
        virtual void SetSize(uint32 width, uint32 height) {}
        virtual void SetPosition(uint32 x, uint32 y) {}
        
        virtual uint32 GetWidth() const { return 0; }
        virtual uint32 GetHeight() const { return 0; }
        virtual void GetPosition(uint32& width, uint32& height) const { }
        virtual void* GetNativeHandle() const { return nullptr; }
	};
}
 
