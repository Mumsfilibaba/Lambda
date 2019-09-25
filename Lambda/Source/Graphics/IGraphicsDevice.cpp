#include "LambdaPch.h"
#include "Graphics/IGraphicsDevice.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "../Platform/DX12/DX12GraphicsDevice.h"
#endif
#include "../Platform/Vulkan/VKNGraphicsDevice.h"

namespace Lambda
{
    //---------------
    //IGraphicsDevice
    //---------------

	IGraphicsDevice* IGraphicsDevice::s_pInstance = nullptr;

	IGraphicsDevice* IGraphicsDevice::Get()
	{
		return s_pInstance;
	}
}
