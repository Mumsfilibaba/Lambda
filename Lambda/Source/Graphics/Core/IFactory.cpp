#include "LambdaPch.h"
#include "Graphics/Core/IFactory.h"
#if defined(LAMBDA_PLAT_WINDOWS)
    //TODO: Include DX12 here
#endif
#include "../../Platform/Vulkan/VKNFactory.h"

namespace Lambda
{
    //--------
    //IFactory
    //--------

    void IFactory::CreateFactory(IFactory **ppFactory, GraphicsAPI api)
    {
        if (api == GRAPHICS_API_VULKAN)
        {
            LOG_ENGINE_INFO("Created Vulkan-Factory\n");
            (*ppFactory) = DBG_NEW VKNFactory();
        }
        else if (api == GRAPHICS_API_D3D12)
        {
#if defined(LAMBDA_PLAT_WINDOWS)
            //TODO: Return DX12 here 
#else
			LOG_ENGINE_INFO("GRAPHICS_API_D3D12 is only available on the Windows platform\n");
            (*ppFactory) = nullptr;
#endif
        }
        else
        {
			LOG_ENGINE_ERROR("Unknown GraphicsAPI\n");
            (*ppFactory) = nullptr;
        }
    }
}
