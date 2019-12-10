#include "LambdaPch.h"
#include "VKNFactory.h"
#include "VKNDevice.h"
#include "VKNSwapChain.h"

namespace Lambda
{
    //----------
    //VKNFactory
    //----------

	VKNFactory::VKNFactory()
	{
		this->AddRef();
	}


	void VKNFactory::CreateDeviceAndSwapChain(IDevice** ppDevice, const SDeviceDesc& deviceDesc, IDeviceContext** ppImmediateContext, ISwapChain** ppSwapChain, const SSwapChainDesc& swapChainDesc)
    {
        LAMBDA_ASSERT_PRINT(ppDevice != nullptr, "[Vulkan] VKNFactory::CreateDeviceAndSwapChain: ppDevice cannot be nullptr\n");
        LAMBDA_ASSERT_PRINT(ppSwapChain != nullptr, "[Vulkan] VKNFactory::CreateDeviceAndSwapChain: ppSwapChain cannot be nullptr\n");
        
        //Create device and swapchain
		VKNDevice* pVkDevice = DBG_NEW VKNDevice(deviceDesc);
		(*ppDevice)		= pVkDevice;
        (*ppSwapChain)  = DBG_NEW VKNSwapChain(pVkDevice, swapChainDesc);
        
        //Check if we are gonna get the immediate context aswell
        if (ppImmediateContext)
            (*ppImmediateContext) = (*ppDevice)->GetImmediateContext();
    }
}
