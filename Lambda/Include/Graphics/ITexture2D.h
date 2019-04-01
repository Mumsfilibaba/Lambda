#pragma once
#include <IObject.h>

namespace Lambda
{
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
		float ClearColor[4];
		float ClearDepth = 1.0f;
		uint8 ClearStencil = 0;
	};


	class ITexture2D : public IObject
	{
	public:
		LAMBDA_INTERFACE(ITexture2D);

		ITexture2D() = default;
		~ITexture2D() = default;
	};
}