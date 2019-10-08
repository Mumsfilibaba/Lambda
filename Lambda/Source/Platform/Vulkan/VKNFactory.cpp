#include "LambdaPch.h"
#include "VKNFactory.h"
#include "VKNDevice.h"
#include "VKNSwapChain.h"

namespace Lambda
{
    //----------
    //VKNFactory
    //----------

    void VKNFactory::CreateDeviceAndSwapChain(IDevice** ppDevice, const DeviceDesc& deviceDesc, IDeviceContext** ppImmediateContext, ISwapChain** ppSwapChain, const SwapChainDesc& swapChainDesc)
    {
        LAMBDA_ASSERT_PRINT(ppDevice != nullptr, "VKNFactory::CreateDeviceAndSwapChain: ppDevice cannot be nullptr\n");
        LAMBDA_ASSERT_PRINT(ppSwapChain != nullptr, "VKNFactory::CreateDeviceAndSwapChain: ppSwapChain cannot be nullptr\n");
        
        //Create device and swapchain
        (*ppDevice)     = DBG_NEW VKNDevice(deviceDesc);
        (*ppSwapChain)  = DBG_NEW VKNSwapChain(swapChainDesc);
        
        //Check if we are gonna get the immediate context aswell
        if (ppImmediateContext)
            (*ppImmediateContext) = (*ppDevice)->GetImmediateContext();
    }
}
