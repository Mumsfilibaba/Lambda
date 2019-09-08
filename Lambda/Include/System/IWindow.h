#pragma once
#include "EventDispatcher.h"

namespace Lambda
{
    class IGraphicsDevice;
    
    
	struct WindowDesc
	{
		const char* pTitle = nullptr;
        GraphicsApi GraphicsDeviceAPI = GRAPHICS_API_VULKAN;
		uint32 Width = 960;
		uint32 Height = 540;
		uint32 SampleCount = 1;
		bool Fullscreen = false;
	};

    
	class LAMBDA_API IWindow
	{
	public:
		LAMBDA_INTERFACE(IWindow);
		
		IWindow() = default;
		virtual ~IWindow() = default;

		virtual void SetEventCallback(EventCallback callback) = 0;
		virtual void OnUpdate() const = 0;

        virtual IGraphicsDevice* GetGraphicsDevice() const = 0;
		virtual uint32 GetHeight() const = 0;
		virtual uint32 GetWidth() const = 0;
		virtual void* GetNativeHandle() const = 0;

	public:
		static IWindow* Create(const WindowDesc& desc);
	};
}
