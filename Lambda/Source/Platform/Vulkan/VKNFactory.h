#pragma once
#include "Graphics/Core/IFactory.h"
#include "Core/CRefCountBase.h"

namespace Lambda
{
    //----------
    //VKNFactory
    //----------

    class VKNFactory final : public CRefCountBase<IFactory>
    {
    public:
        LAMBDA_NO_COPY(VKNFactory);
        
        VKNFactory();
        ~VKNFactory() = default;
        
        virtual void CreateDeviceAndSwapChain(IDevice** ppDevice, const SDeviceDesc& deviceDesc, IDeviceContext** pImmediateContext, ISwapChain** ppSwapChain, const SSwapChainDesc& swapChainDesc) override final;
    };
}
