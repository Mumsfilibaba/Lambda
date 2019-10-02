#pragma once
#include "IObject.h"

namespace Lambda
{
    class ITexture;
	class IDevice;

    //-------------------
    //OptimizedClearValue
    //-------------------
    
    struct OptimizedClearValue
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
    
    //-----------
    //TextureDesc
    //-----------
    
	struct TextureDesc
	{
		TextureType Type            = TEXTURE_TYPE_UNKNOWN;
        ResourceUsage Usage         = RESOURCE_USAGE_UNKNOWN;
        Format Format       = FORMAT_UNKNOWN;
        ITexture* pResolveResource  = nullptr;
        uint32 Flags                = TEXTURE_FLAGS_NONE;
		uint32 SampleCount          = 1;
		uint32 MipLevels            = 0;
		uint32 ArraySize            = 1;
		uint32 Width                = 0;
		uint32 Height               = 0;
		uint32 Depth                = 1;
	};

    //--------
    //ITexture
    //--------
    
	class LAMBDA_API ITexture : public IObject
	{
	public:
		LAMBDA_IOBJECT_INTERFACE(ITexture);

        virtual void* GetNativeHandle() const = 0;
		virtual const TextureDesc& GetDesc() const = 0;
	public:
		static ITexture* CreateTextureFromFile(IDevice* pDevice, const char* pFileName, uint32 flags, ResourceUsage Usage, Format Format);
	};
}