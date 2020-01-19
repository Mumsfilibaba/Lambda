#pragma once
#include "LambdaCore.h"

#include "Core/Engine/ISharedObject.h"

#if defined(IsMaximized)
    #undef IsMaximized
#endif

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

    //------------
    //SWindowProps
    //------------

    struct SWindowProps
    {
        const char* pTitle = "";
        uint32 Width  = 0;
        uint32 Height = 0;
    };

	//-------
	//CWindow
	//-------
	
    class CWindow : public CSharedObject<ISharedObject>
	{
    protected:
        LAMBDA_NO_COPY(CWindow);
        
        CWindow()  = default;
        ~CWindow() = default;
    public:
        virtual void SetDisplayMode(EDisplayMode) {}
        virtual EDisplayMode GetDisplayMode() const { return EDisplayMode::DISPLAY_MODE_WINDOWED; };
        
        virtual void Show() {}
        virtual void Hide() {}
        virtual void Maximize() {}
        virtual void Minimize() {}
        
        virtual bool HasFocus() const       { return false; }
        virtual bool IsFullscreen() const   { return false; }
        virtual bool IsMaximized() const    { return false; }
        virtual bool IsMiniaturized() const { return false; }
        
        virtual void SetFocus() {}
        virtual void SetSize(uint32, uint32) {}
        virtual void SetPosition(uint32, uint32) {}
        
        virtual uint32 GetWidth() const  { return 0; }
        virtual uint32 GetHeight() const { return 0; }
        virtual void* GetNativeHandle() const { return nullptr; }
        
        virtual void GetPosition(uint32& width, uint32& height) const 
        {
            width  = 0;
            height = 0;
        }
	};
}
 
