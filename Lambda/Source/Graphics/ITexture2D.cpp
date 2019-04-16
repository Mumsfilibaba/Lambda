#include <LambdaPch.h>
#include <Graphics/ITexture2D.h>
#include <Graphics/IGraphicsDevice.h>
#include <Utilities/TextureHelper.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Lambda
{
	ITexture2D* ITexture2D::CreateTextureFromFile(const IGraphicsDevice* pDevice, const char* pFileName, TextureFlags flags, ResourceUsage usage, ResourceFormat format)
	{
		int32 width = 0;
		int32 height = 0;
		int32 components = 0;
		int32 reqComponents = ComponentCountFromResourceFormat(format);
		ITexture2D* pTexture = nullptr;

		//Load
		uint8* pTexData = stbi_load(pFileName, &width, &height, &components, reqComponents);
		if (pTexData != nullptr)
		{
			//Setup desc
			Texture2DDesc desc = {};
			desc.ArraySize = 1;
			desc.Flags = flags;
			desc.Format = format;
			desc.Height = height;
			desc.Width = width;
			desc.MipLevels = 0;
			desc.SampleCount = 1;
			desc.Usage = usage;

			//Create texture from loaded data
			ResourceData data = {};
			data.pData = (void*)pTexData;
			data.SizeInBytes = width * height * StrideInBytesFromResourceFormat(format);

			pDevice->CreateTexture2D(&pTexture, &data, desc);
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