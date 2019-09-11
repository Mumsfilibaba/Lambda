#pragma once
#include "Types.h"

namespace Lambda
{
	inline uint32 ComponentCountFromResourceFormat(ResourceFormat format)
	{
		switch (format)
		{
			//4 components
		case FORMAT_R32G32B32A32_FLOAT:
		case FORMAT_R32G32B32A32_UINT:
		case FORMAT_R32G32B32A32_SINT:
		case FORMAT_R16G16B16A16_FLOAT:
		case FORMAT_R16G16B16A16_UNORM:
		case FORMAT_R16G16B16A16_UINT:	
		case FORMAT_R16G16B16A16_SNORM:
		case FORMAT_R16G16B16A16_SINT:
		case FORMAT_R8G8B8A8_UNORM:		
		case FORMAT_R8G8B8A8_UNORM_SRGB:
		case FORMAT_R8G8B8A8_UINT:		
		case FORMAT_R8G8B8A8_SNORM:		
		case FORMAT_R8G8B8A8_SINT:		
		case FORMAT_B8G8R8A8_UNORM:
		case FORMAT_B8G8R8A8_UNORM_SRGB:
			return 4;

			//3 components
		case FORMAT_R32G32B32_FLOAT:
		case FORMAT_R32G32B32_UINT:	
		case FORMAT_R32G32B32_SINT:
			return 3;

			//2 components
		case FORMAT_R32G32_FLOAT:			
		case FORMAT_R32G32_UINT:			
		case FORMAT_R32G32_SINT:
		case FORMAT_R16G16_FLOAT:		
		case FORMAT_R16G16_UNORM:		
		case FORMAT_R16G16_UINT:		
		case FORMAT_R16G16_SNORM:		
		case FORMAT_R16G16_SINT:		
		case FORMAT_R8G8_UNORM:
		case FORMAT_R8G8_UINT:	
		case FORMAT_R8G8_SNORM:
		case FORMAT_R8G8_SINT:	
			return 2;

			//1 component
		case FORMAT_D32_FLOAT:
		case FORMAT_R32_FLOAT:
		case FORMAT_R32_UINT:
		case FORMAT_R32_SINT:
		case FORMAT_R16_FLOAT:
		case FORMAT_D16_UNORM:
		case FORMAT_R16_UNORM:
		case FORMAT_R16_UINT:
		case FORMAT_R16_SNORM:
		case FORMAT_R16_SINT:
		case FORMAT_R8_UNORM:
		case FORMAT_R8_UINT:
		case FORMAT_R8_SNORM:
		case FORMAT_R8_SINT:
			return 1;

			//No components
		case FORMAT_D32_FLOAT_S8X24_UINT:
		case FORMAT_D24_UNORM_S8_UINT:
		default:
			return 0;
		}
	}


	inline uint32 StrideInBytesFromResourceFormat(ResourceFormat format)
	{
		switch (format)
		{
			//16 bytes
		case FORMAT_R32G32B32A32_FLOAT:
		case FORMAT_R32G32B32A32_UINT:
		case FORMAT_R32G32B32A32_SINT:
			return 16;

			//12 components
		case FORMAT_R32G32B32_FLOAT:
		case FORMAT_R32G32B32_UINT:
		case FORMAT_R32G32B32_SINT:
			return 12;

			//8 bytes
		case FORMAT_D32_FLOAT_S8X24_UINT:
		case FORMAT_R16G16B16A16_FLOAT:
		case FORMAT_R16G16B16A16_UNORM:
		case FORMAT_R16G16B16A16_UINT:
		case FORMAT_R16G16B16A16_SNORM:
		case FORMAT_R16G16B16A16_SINT:
		case FORMAT_R32G32_FLOAT:
		case FORMAT_R32G32_UINT:
		case FORMAT_R32G32_SINT:
			return 8;

			//4 bytes
		case FORMAT_D24_UNORM_S8_UINT:
		case FORMAT_R8G8B8A8_UNORM:
		case FORMAT_R8G8B8A8_UNORM_SRGB:
		case FORMAT_R8G8B8A8_UINT:
		case FORMAT_R8G8B8A8_SNORM:
		case FORMAT_R8G8B8A8_SINT:
		case FORMAT_B8G8R8A8_UNORM:
		case FORMAT_B8G8R8A8_UNORM_SRGB:
		case FORMAT_R16G16_FLOAT:
		case FORMAT_R16G16_UNORM:
		case FORMAT_R16G16_UINT:
		case FORMAT_R16G16_SNORM:
		case FORMAT_R16G16_SINT:
		case FORMAT_D32_FLOAT:
		case FORMAT_R32_FLOAT:
		case FORMAT_R32_UINT:
		case FORMAT_R32_SINT:
			return 4;

			//2 bytes
		case FORMAT_R8G8_UNORM:
		case FORMAT_R8G8_UINT:
		case FORMAT_R8G8_SNORM:
		case FORMAT_R8G8_SINT:
		case FORMAT_R16_FLOAT:
		case FORMAT_D16_UNORM:
		case FORMAT_R16_UNORM:
		case FORMAT_R16_UINT:
		case FORMAT_R16_SNORM:
		case FORMAT_R16_SINT:
			return 2;

			//1 byte
		case FORMAT_R8_UNORM:
		case FORMAT_R8_UINT:
		case FORMAT_R8_SNORM:
		case FORMAT_R8_SINT:
			return 1;

			//No bytes
		default:
			return 0;
		}
	}
}