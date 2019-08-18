#pragma once
#include "EventDispatcher.h"

namespace Lambda
{
    //Forward declarations
    class IGraphicsDevice;
    
    
    //Window desceription
	struct WindowDesc
	{
		const char* pTitle = nullptr;
		uint32 Width = 960;
		uint32 Height = 540;
        GraphicsApi GraphicsDeviceAPI = GRAPHICS_API_VULKAN;
	};

    
    //Window class
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
