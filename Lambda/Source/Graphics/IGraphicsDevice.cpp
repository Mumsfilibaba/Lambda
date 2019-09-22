#include "LambdaPch.h"
#include "Graphics/IGraphicsDevice.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "../Platform/DX12/DX12GraphicsDevice.h"
#endif
#include "../Platform/Vulkan/VKNGraphicsDevice.h"

namespace Lambda
{

    //-------------
    //GraphicsLayer
    //-------------

    GraphicsLayer::GraphicsLayer()
        : EventLayer("GraphicsLayer")
    {
    }


    void GraphicsLayer::OnPop()
    {
    }


    void GraphicsLayer::OnPush()
    {
    }


    bool GraphicsLayer::OnEvent(const Event* pEvent)
    {
        IGraphicsDevice* pDevice = IGraphicsDevice::Get();
        return pDevice->OnEvent(pEvent);
    }


    uint32 GraphicsLayer::GetRecivableCategories() const
    {
        return EVENT_CATEGORY_WINDOW;
    }

    //---------------
    //IGraphicsDevice
    //---------------

	IGraphicsDevice* IGraphicsDevice::s_pInstance = nullptr;

#if defined(LAMBDA_PLAT_WINDOWS)
	IGraphicsDevice* IGraphicsDevice::Create(const GraphicsDeviceDesc& desc)
	{
		if (s_pInstance)
		{
			LOG_DEBUG_WARNING("GraphicsDevice already created\n");
			return s_pInstance;
		}

		if (desc.Api == GRAPHICS_API_D3D12)
		{
			return DBG_NEW DX12GraphicsDevice(desc);
		}
		else if (desc.Api == GRAPHICS_API_VULKAN)
		{
			return DBG_NEW VKNGraphicsDevice(desc);
		}

		return nullptr;
	}
#elif defined(LAMBDA_PLAT_MACOS)
	IGraphicsDevice* IGraphicsDevice::Create(const GraphicsDeviceDesc& desc)
	{
		if (s_pInstance)
		{
			LOG_DEBUG_WARNING("GraphicsDevice already created\n");
			return s_pInstance;
		}

		if (desc.Api == GRAPHICS_API_D3D12)
		{
			LOG_DEBUG_ERROR("Lambda Engine: D3D12 is only supported on the Windows-Platform\n");
			return nullptr;
		}
		else if (desc.Api == GRAPHICS_API_VULKAN)
		{
			return DBG_NEW VKNGraphicsDevice(desc);
		}

		return nullptr;
	}
#endif


	IGraphicsDevice* IGraphicsDevice::Get()
	{
		return s_pInstance;
	}
}
