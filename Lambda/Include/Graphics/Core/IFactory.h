#pragma once
#include "IObject.h"

namespace Lambda
{
    class IDevice;
    class ISwapChain;
    class IDeviceContext;

    struct SDeviceDesc;
    struct SSwapChainDesc;

    //--------
    //IFactory
    //--------

    class LAMBDA_API IFactory : public IObject
    {
    public:
        LAMBDA_IOBJECT_INTERFACE(IFactory);
        
        virtual void CreateDeviceAndSwapChain(IDevice** ppDevice, const SDeviceDesc& deviceDesc, IDeviceContext** pImmediateContext, ISwapChain** ppSwapChain, const SSwapChainDesc& swapChainDesc) = 0;
    public:
        static void CreateFactory(IFactory** ppFactory, EGraphicsAPI api);
    };
}
