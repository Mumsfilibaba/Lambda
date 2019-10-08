#pragma once
#include "IObject.h"

namespace Lambda
{
    class IDevice;
    class IDeviceContext;
    class ISwapChain;

    struct DeviceDesc;
    struct SwapChainDesc;

    //--------
    //IFactory
    //--------

    class LAMBDA_API IFactory : public IObject
    {
    public:
        LAMBDA_IOBJECT_INTERFACE(IFactory);
        
        virtual void CreateDeviceAndSwapChain(IDevice** ppDevice, const DeviceDesc& deviceDesc, IDeviceContext** pImmediateContext, ISwapChain** ppSwapChain, const SwapChainDesc& swapChainDesc) = 0;
        
    public:
        static void CreateFactory(IFactory** ppFactory, GraphicsAPI api);
    };
}
