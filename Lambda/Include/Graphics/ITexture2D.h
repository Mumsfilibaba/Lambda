#pragma once
#include "IObject.h"

namespace Lambda
{
	class IGraphicsDevice;


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
    
    
	struct Texture2DDesc
	{
		ResourceUsage Usage = RESOURCE_USAGE_UNKNOWN;
		ResourceFormat Format = FORMAT_UNKNOWN;
		uint32 Flags = TEXTURE_FLAGS_NONE;
		uint32 SampleCount = 1;
		uint32 MipLevels = 0;
		uint32 ArraySize = 1;
		uint32 Width = 0;
		uint32 Height = 0;
        OptimizedClearValue ClearValue;
	};


	class LAMBDA_API ITexture2D
	{
	public:
		LAMBDA_INTERFACE(ITexture2D);

		ITexture2D() = default;
		~ITexture2D() = default;

        virtual void* GetNativeHandle() const = 0;
		virtual Texture2DDesc GetDesc() const = 0;
        virtual uint32 GetMipLevels() const = 0;
        virtual uint32 GetWidth() const = 0;
        virtual uint32 GetHeight() const = 0;

	public:
		static ITexture2D* CreateTextureFromFile(const IGraphicsDevice* pDevice, const char* pFileName, uint32 flags, ResourceUsage Usage, ResourceFormat Format);
	};
}
