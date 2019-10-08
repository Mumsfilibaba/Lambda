#pragma once
#include "IDeviceObject.h"

namespace Lambda
{
    class ITexture;

    //-------------
    //SwapChainDesc
    //-------------

    struct SwapChainDesc
    {
        void* pWindowHandle         = nullptr; //HWND on Windows, GLFWWindow* on macOS
        Format BufferFormat         = FORMAT_UNKNOWN;
        Format DepthBufferFormat    = FORMAT_UNKNOWN;
        uint32 BufferCount          = 3;
        uint32 BufferSampleCount    = 1;
        uint32 BufferWidth          = 0;
        uint32 BufferHeight         = 0;
        bool VerticalSync           = true;
    };

    //----------
    //ISwapChain
    //----------

    class LAMBDA_API ISwapChain : public IDeviceObject
    {
    public:
        LAMBDA_IOBJECT_INTERFACE(ISwapChain);
        
        virtual void ResizeBuffers(uint32 width, uint32 height) = 0;
        virtual ITexture* GetBuffer() = 0;
        virtual ITexture* GetDepthBuffer() = 0;
        virtual void Present() = 0;
        virtual void* GetNativeHandle() const = 0;
        virtual const SwapChainDesc& GetDesc() const = 0;
    };
}
