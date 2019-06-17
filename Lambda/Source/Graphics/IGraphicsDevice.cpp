#include "LambdaPch.h"
#include "Graphics/IGraphicsDevice.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "../Platform/DX12/DX12GraphicsDevice.h"
#endif
#include "../Platform/Vulkan/VulkanGraphicsDevice.h"

namespace Lambda
{
	IGraphicsDevice* IGraphicsDevice::s_pInstance = nullptr;

#if defined(LAMBDA_PLAT_WINDOWS)
	IGraphicsDevice* IGraphicsDevice::Create(IWindow* pWindow, const GraphicsDeviceDesc& desc)
	{
		if (desc.Api == GRAPHICS_API_D3D12)
		{
			return DBG_NEW DX12GraphicsDevice(pWindow, desc);
		}
		else if (desc.Api == GRAPHICS_API_VULKAN)
		{
			return DBG_NEW VulkanGraphicsDevice(pWindow, desc);
		}

		return nullptr;
	}
#elif defined(LAMBDA_PLAT_MACOS)
	IGraphicsDevice* IGraphicsDevice::Create(IWindow* pWindow, const GraphicsDeviceDesc& desc)
	{
		if (desc.Api == GRAPHICS_API_D3D12)
		{
			LOG_DEBUG_ERROR("Lambda Engine: D3D12 is only supported on the Windows-Platform\n");
			return nullptr;
		}
		else if (desc.Api == GRAPHICS_API_VULKAN)
		{
			return DBG_NEW VulkanGraphicsDevice(pWindow, desc);
		}

		return nullptr;
	}
#endif


	IGraphicsDevice* IGraphicsDevice::GetInstance()
	{
		return s_pInstance;
	}


	bool IGraphicsDevice::OnEvent(const Event& event)
	{
		return (s_pInstance) ? s_pInstance->InternalOnEvent(event) : false;
	}
}
