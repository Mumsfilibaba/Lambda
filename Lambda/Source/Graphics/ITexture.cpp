#include "LambdaPch.h"
#include "Graphics/ITexture.h"
#include "Graphics/IGraphicsDevice.h"
#include "Utilities/TextureHelper.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Lambda
{
	ITexture* ITexture::CreateTextureFromFile(IGraphicsDevice* pDevice, const char* pFileName, uint32 flags, ResourceUsage usage, ResourceFormat format)
	{
		int32 width = 0;
		int32 height = 0;
		int32 components = 0;
		int32 reqComponents = ComponentCountFromResourceFormat(format);
		ITexture* pTexture = nullptr;

		//Load
		uint8* pTexData = stbi_load(pFileName, &width, &height, &components, reqComponents);
		if (pTexData != nullptr)
		{
			//Setup desc
			TextureDesc desc = {};
			desc.Type = TEXTURE_TYPE_2D;
			desc.ArraySize = 1;
			desc.Flags = flags;
			desc.Format = format;
			desc.Height = height;
			desc.Width = width;
			desc.MipLevels = 1;
			desc.SampleCount = 1;
			desc.Usage = usage;

			//Create texture from loaded data
			ResourceData data = {};
			data.pData = reinterpret_cast<void*>(pTexData);
			data.SizeInBytes = uint64(width) * uint64(height) * uint64(StrideInBytesFromResourceFormat(format));

			pDevice->CreateTexture(&pTexture, &data, desc);
			SafeDelete(pTexData);

			LOG_DEBUG_INFO("Lambda Engine: Loaded texture '%s'\n", pFileName);
		}
		else
		{
			LOG_DEBUG_ERROR("Lambda Engine: Failed to load texture '%s'\n", pFileName);
		}

		return pTexture;
	}
}
