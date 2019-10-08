#pragma once
#include "Graphics/Core/IFactory.h"

namespace Lambda
{
    //----------
    //VKNFactory
    //----------

    class VKNFactory final : public RefCountedObject<IFactory>
    {
    public:
        LAMBDA_NO_COPY(VKNFactory);
        
        VKNFactory() = default;
        ~VKNFactory() = default;
        
        virtual void CreateDeviceAndSwapChain(IDevice** ppDevice, const DeviceDesc& deviceDesc, IDeviceContext** pImmediateContext, ISwapChain** ppSwapChain, const SwapChainDesc& swapChainDesc) override final;
    };
}
