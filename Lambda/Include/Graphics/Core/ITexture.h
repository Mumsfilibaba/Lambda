#pragma once
#include "IDeviceObject.h"

namespace Lambda
{
    class ITexture;
	class IDevice;

    //--------------------
    //SOptimizedClearValue
    //--------------------
    
    struct SOptimizedClearValue
    {
        union
        {
            float Color[4];
            struct
            {
                float Depth;
                uint8 Stencil;
            };
        };
    };
    
    //------------
    //STextureDesc
    //------------
    
	struct STextureDesc
	{
		const char* pName	= nullptr;
		ETextureType Type   = TEXTURE_TYPE_UNKNOWN;
        EUsage Usage		= USAGE_UNKNOWN;
        EFormat Format      = FORMAT_UNKNOWN;
        uint32 Flags        = TEXTURE_FLAGS_NONE;
		uint32 SampleCount  = 1;
		uint32 MipLevels    = 0;
		uint32 ArraySize    = 1;
		uint32 Width        = 0;
		uint32 Height       = 0;
		uint32 Depth        = 1;
	};

    //--------
    //ITexture
    //--------
    
	class LAMBDA_API ITexture : public IDeviceObject
	{
	public:
		LAMBDA_IOBJECT_INTERFACE(ITexture);

        virtual void* GetNativeHandle() const = 0;
		virtual const STextureDesc& GetDesc() const = 0;
	public:
		static ITexture* CreateTextureFromFile(IDevice* pDevice, const char* pFileName, uint32 flags, EUsage Usage, EFormat Format);
	};
}
