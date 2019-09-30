#include "LambdaPch.h"
#include "Graphics/IDevice.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "../Platform/DX12/DX12GraphicsDevice.h"
#endif
#include "../Platform/Vulkan/VKNDevice.h"

namespace Lambda
{
    //---------------
    //IDevice
    //---------------

	IDevice* IDevice::s_pInstance = nullptr;

	IDevice* IDevice::Get()
	{
        LAMBDA_ASSERT(s_pInstance != nullptr);
		return s_pInstance;
	}
}
