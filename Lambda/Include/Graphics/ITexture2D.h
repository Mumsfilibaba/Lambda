#pragma once
#include <IObject.h>

namespace Lambda
{
	struct Texture2DDesc
	{
		ResourceUsage Usage = RESOURCE_USAGE_UNKNOWN;
		TextureFlags Flags = TEXTURE_FLAGS_NONE;
		ResourceFormat Format = FORMAT_UNKNOWN;
		uint32 Samples = 1;
		uint32 MipMaps = 0;
		uint32 ArraySize = 1;
		uint32 Width = 0;
		uint32 Height = 0;
	};


	class ITexture2D : public IObject
	{
	public:
		LAMBDA_INTERFACE(ITexture2D);

		ITexture2D() = default;
		~ITexture2D() = default;
	};
}