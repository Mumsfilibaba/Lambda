#pragma once
#include "Events/EventDispatcher.h"

namespace Lambda
{
    class IDevice;
    
    //----------
    //WindowDesc
    //----------
    
	struct WindowDesc
	{
		const char* pTitle              = nullptr;
        GraphicsApi GraphicsDeviceAPI   = GRAPHICS_API_VULKAN;
		uint32 Width                    = 960;
		uint32 Height                   = 540;
		uint32 SampleCount              = 1;
		bool Fullscreen                 = false;
        bool VerticalSync               = true;
	};

    //-------
    //IWindow
    //-------
    
	class LAMBDA_API IWindow
	{
	public:
		LAMBDA_INTERFACE(IWindow);

        virtual void OnUpdate() const = 0;
        virtual bool HasFocus() const = 0;
        virtual bool SetFullscreen(bool fullscreen) = 0;
        virtual void SetEventCallback(IEventCallback* pEventCallback) = 0;
        virtual bool GetFullscreen() const = 0;
        virtual const IDevice* GetGraphicsDevice() const = 0;
		virtual uint32 GetHeight() const = 0;
		virtual uint32 GetWidth() const = 0;
		virtual void* GetNativeHandle() const = 0;
	public:
		static IWindow* Create(const WindowDesc& desc);
	};
}
