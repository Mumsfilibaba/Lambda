#pragma once
#include "Types.h"

namespace Lambda
{
    inline VkFormat ConvertFormat(Format format)
    {
        switch (format)
        {
            case FORMAT_R32G32B32A32_FLOAT:     return VK_FORMAT_R32G32B32A32_SFLOAT;
            case FORMAT_R32G32B32A32_UINT:      return VK_FORMAT_R32G32B32A32_UINT;
            case FORMAT_R32G32B32A32_SINT:      return VK_FORMAT_R32G32B32A32_SINT;
            case FORMAT_R32G32B32_FLOAT:        return VK_FORMAT_R32G32B32_SFLOAT;
            case FORMAT_R32G32B32_UINT:         return VK_FORMAT_R32G32B32_UINT;
            case FORMAT_R32G32B32_SINT:         return VK_FORMAT_R32G32B32_SINT;
            case FORMAT_R16G16B16A16_FLOAT:     return VK_FORMAT_R16G16B16A16_SFLOAT;
            case FORMAT_R16G16B16A16_UNORM:     return VK_FORMAT_R16G16B16A16_UNORM;
            case FORMAT_R16G16B16A16_UINT:      return VK_FORMAT_R16G16B16A16_UINT;
            case FORMAT_R16G16B16A16_SNORM:     return VK_FORMAT_R16G16B16A16_SNORM;
            case FORMAT_R16G16B16A16_SINT:      return VK_FORMAT_R16G16B16A16_SINT;
            case FORMAT_R32G32_FLOAT:           return VK_FORMAT_R32G32_SFLOAT;
            case FORMAT_R32G32_UINT:            return VK_FORMAT_R32G32_UINT;
            case FORMAT_R32G32_SINT:            return VK_FORMAT_R32G32_SINT;
            case FORMAT_R8G8B8A8_UNORM:         return VK_FORMAT_R8G8B8A8_UNORM;
            case FORMAT_R8G8B8A8_UNORM_SRGB:    return VK_FORMAT_R8G8B8A8_SRGB;
            case FORMAT_R8G8B8A8_UINT:          return VK_FORMAT_R8G8B8A8_UINT;
            case FORMAT_R8G8B8A8_SNORM:         return VK_FORMAT_R8G8B8A8_SNORM;
            case FORMAT_R8G8B8A8_SINT:          return VK_FORMAT_R8G8B8A8_SINT;
            case FORMAT_R16G16_FLOAT:           return VK_FORMAT_R16G16_SFLOAT;
            case FORMAT_R16G16_UNORM:           return VK_FORMAT_R16G16_UNORM;
            case FORMAT_R16G16_UINT:            return VK_FORMAT_R16G16_UINT;
            case FORMAT_R16G16_SNORM:           return VK_FORMAT_R16G16_SNORM;
            case FORMAT_R16G16_SINT:            return VK_FORMAT_R16G16_SINT;
            case FORMAT_D32_FLOAT:              return VK_FORMAT_D32_SFLOAT;
            case FORMAT_R32_FLOAT:              return VK_FORMAT_R32_SFLOAT;
            case FORMAT_R32_UINT:               return VK_FORMAT_R32_UINT;
            case FORMAT_R32_SINT:               return VK_FORMAT_R32_SINT;
            case FORMAT_D32_FLOAT_S8X24_UINT:   return VK_FORMAT_D32_SFLOAT_S8_UINT;
            case FORMAT_D24_UNORM_S8_UINT:      return VK_FORMAT_D24_UNORM_S8_UINT;
            case FORMAT_R8G8_UNORM:             return VK_FORMAT_R8G8_UNORM;
            case FORMAT_R8G8_UINT:              return VK_FORMAT_R8G8_UINT;
            case FORMAT_R8G8_SNORM:             return VK_FORMAT_R8G8_SNORM;
            case FORMAT_R8G8_SINT:              return VK_FORMAT_R8G8_SINT;
            case FORMAT_R16_FLOAT:              return VK_FORMAT_R16_SFLOAT;
            case FORMAT_D16_UNORM:              return VK_FORMAT_D16_UNORM;
            case FORMAT_R16_UNORM:              return VK_FORMAT_R16_UNORM;
            case FORMAT_R16_UINT:               return VK_FORMAT_R16_UINT;
            case FORMAT_R16_SNORM:              return VK_FORMAT_R16_SNORM;
            case FORMAT_R16_SINT:               return VK_FORMAT_R16_SINT;
            case FORMAT_R8_UNORM:               return VK_FORMAT_R8_UNORM;
            case FORMAT_R8_UINT:                return VK_FORMAT_R8_UINT;
            case FORMAT_R8_SNORM:               return VK_FORMAT_R8_SNORM;
            case FORMAT_R8_SINT:                return VK_FORMAT_R8_SINT;
            case FORMAT_B8G8R8A8_UNORM:         return VK_FORMAT_B8G8R8A8_UNORM;
            case FORMAT_B8G8R8A8_UNORM_SRGB:    return VK_FORMAT_B8G8R8A8_SRGB;
            case FORMAT_UNKNOWN:
            default: return VK_FORMAT_UNDEFINED;
        }
    }
    
    
    inline Format ConvertVkFormat(VkFormat vkFormat)
    {
        switch (vkFormat)
        {
            case VK_FORMAT_R32G32B32A32_SFLOAT:    return FORMAT_R32G32B32A32_FLOAT;
            case VK_FORMAT_R32G32B32A32_UINT:      return FORMAT_R32G32B32A32_UINT;
            case VK_FORMAT_R32G32B32A32_SINT:      return FORMAT_R32G32B32A32_SINT;
            case VK_FORMAT_R32G32B32_SFLOAT:       return FORMAT_R32G32B32_FLOAT;
            case VK_FORMAT_R32G32B32_UINT:         return FORMAT_R32G32B32_UINT;
            case VK_FORMAT_R32G32B32_SINT:         return FORMAT_R32G32B32_SINT;
            case VK_FORMAT_R16G16B16A16_SFLOAT:    return FORMAT_R16G16B16A16_FLOAT;
            case VK_FORMAT_R16G16B16A16_UNORM:     return FORMAT_R16G16B16A16_UNORM;
            case VK_FORMAT_R16G16B16A16_UINT:      return FORMAT_R16G16B16A16_UINT;
            case VK_FORMAT_R16G16B16A16_SNORM:     return FORMAT_R16G16B16A16_SNORM;
            case VK_FORMAT_R16G16B16A16_SINT:      return FORMAT_R16G16B16A16_SINT;
            case VK_FORMAT_R32G32_SFLOAT:          return FORMAT_R32G32_FLOAT;
            case VK_FORMAT_R32G32_UINT:            return FORMAT_R32G32_UINT;
            case VK_FORMAT_R32G32_SINT:            return FORMAT_R32G32_SINT;
            case VK_FORMAT_R8G8B8A8_UNORM:         return FORMAT_R8G8B8A8_UNORM;
            case VK_FORMAT_R8G8B8A8_SRGB:          return FORMAT_R8G8B8A8_UNORM_SRGB;
            case VK_FORMAT_R8G8B8A8_UINT:          return FORMAT_R8G8B8A8_UINT;
            case VK_FORMAT_R8G8B8A8_SNORM:         return FORMAT_R8G8B8A8_SNORM;
            case VK_FORMAT_R8G8B8A8_SINT:          return FORMAT_R8G8B8A8_SINT;
            case VK_FORMAT_R16G16_SFLOAT:          return FORMAT_R16G16_FLOAT;
            case VK_FORMAT_R16G16_UNORM:           return FORMAT_R16G16_UNORM;
            case VK_FORMAT_R16G16_UINT:            return FORMAT_R16G16_UINT;
            case VK_FORMAT_R16G16_SNORM:           return FORMAT_R16G16_SNORM;
            case VK_FORMAT_R16G16_SINT:            return FORMAT_R16G16_SINT;
            case VK_FORMAT_D32_SFLOAT:             return FORMAT_D32_FLOAT;
            case VK_FORMAT_R32_SFLOAT:             return FORMAT_R32_FLOAT;
            case VK_FORMAT_R32_UINT:               return FORMAT_R32_UINT;
            case VK_FORMAT_R32_SINT:               return FORMAT_R32_SINT;
            case VK_FORMAT_D24_UNORM_S8_UINT:      return FORMAT_D24_UNORM_S8_UINT;
            case VK_FORMAT_R8G8_UNORM:             return FORMAT_R8G8_UNORM;
            case VK_FORMAT_R8G8_UINT:              return FORMAT_R8G8_UINT;
            case VK_FORMAT_R8G8_SNORM:             return FORMAT_R8G8_SNORM;
            case VK_FORMAT_R8G8_SINT:              return FORMAT_R8G8_SINT;
            case VK_FORMAT_R16_SFLOAT:             return FORMAT_R16_FLOAT;
            case VK_FORMAT_D16_UNORM:              return FORMAT_D16_UNORM;
            case VK_FORMAT_R16_UNORM:              return FORMAT_R16_UNORM;
            case VK_FORMAT_R16_UINT:               return FORMAT_R16_UINT;
            case VK_FORMAT_R16_SNORM:              return FORMAT_R16_SNORM;
            case VK_FORMAT_R16_SINT:               return FORMAT_R16_SINT;
            case VK_FORMAT_R8_UNORM:               return FORMAT_R8_UNORM;
            case VK_FORMAT_R8_UINT:                return FORMAT_R8_UINT;
            case VK_FORMAT_R8_SNORM:               return FORMAT_R8_SNORM;
            case VK_FORMAT_R8_SINT:                return FORMAT_R8_SINT;
            case VK_FORMAT_B8G8R8A8_UNORM:         return FORMAT_B8G8R8A8_UNORM;
            case VK_FORMAT_B8G8R8A8_SRGB:          return FORMAT_B8G8R8A8_UNORM_SRGB;
            case VK_FORMAT_UNDEFINED:
            default: return FORMAT_UNKNOWN;
        }
    }
    
    
    inline VkPrimitiveTopology ConvertPrimitiveTopology(PrimitiveTopology toplogy)
    {
        switch (toplogy)
        {
            case PRIMITIVE_TOPOLOGY_POINTLIST: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
			case PRIMITIVE_TOPOLOGY_TRIANGLELIST: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            case PRIMITIVE_TOPOLOGY_TRIANGLESTRIP: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            default: return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
        }
    }
    

	inline VkPolygonMode ConvertPolygonMode(PolygonMode mode)
	{
		switch (mode)
		{
		case POLYGON_MODE_FILL: return VK_POLYGON_MODE_FILL;
		case POLYGON_MODE_LINE: return VK_POLYGON_MODE_LINE;
		case POLYGON_MODE_POINT: return VK_POLYGON_MODE_POINT;
		default: return VK_POLYGON_MODE_MAX_ENUM;
		}
	}


    inline PrimitiveTopology ConvertVkPrimitiveTopology(VkPrimitiveTopology toplogy)
    {
        switch (toplogy)
        {
            case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:	return PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:	return PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            default: return PRIMITIVE_TOPOLOGY_UNKNOWN;
        }
    }
    
    
    inline VkImageLayout ConvertResourceStateToImageLayout(ResourceState state)
    {
        switch (state)
        {
            case RESOURCE_STATE_PIXEL_SHADER_RESOURCE:  return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            case RESOURCE_STATE_RENDERTARGET:           return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            case RESOURCE_STATE_DEPTH_STENCIL:			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            case RESOURCE_STATE_RENDERTARGET_CLEAR:     return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            case RESOURCE_STATE_PRESENT:				return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            case RESOURCE_STATE_DEPTH_STENCIL_CLEAR:    return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            case RESOURCE_STATE_COPY_DEST:              return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            case RESOURCE_STATE_COPY_SRC:               return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			case RESOURCE_STATE_GENERAL:				return VK_IMAGE_LAYOUT_GENERAL;
            case RESOURCE_STATE_UNKNOWN:
            default: return VK_IMAGE_LAYOUT_UNDEFINED;
        }
    }
    
    
    inline VkSamplerAddressMode ConvertSamplerAdressMode(SamplerAddressMode adressMode)
    {
        switch (adressMode) 
		{
            case SAMPLER_ADDRESS_MODE_REPEAT:               return VK_SAMPLER_ADDRESS_MODE_REPEAT;
            case SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT:      return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
            case SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            case SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER:      return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            case SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
            case SAMPLER_ADDRESS_MODE_UNKNOWN:
            default: return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
        }
    }


	inline VkSampleCountFlagBits ConvertSampleCount(uint32 sampleCount)
	{
		switch (sampleCount) 
		{
		case 1:  return VK_SAMPLE_COUNT_1_BIT;
		case 2:  return VK_SAMPLE_COUNT_2_BIT;
		case 4:  return VK_SAMPLE_COUNT_4_BIT;
		case 8:  return VK_SAMPLE_COUNT_8_BIT;
		case 16: return VK_SAMPLE_COUNT_16_BIT;
		case 32: return VK_SAMPLE_COUNT_32_BIT;
		case 64: return VK_SAMPLE_COUNT_64_BIT;
		default: return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
		}
	}
    
    
    inline VkPipelineStageFlagBits ConvertPipelineStage(PipelineStage stage)
    {
        switch (stage)
        {
            case PIPELINE_STAGE_VERTEX:      return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
            case PIPELINE_STAGE_HULL:        return VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
            case PIPELINE_STAGE_DOMAIN:      return VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
            case PIPELINE_STAGE_GEOMETRY:    return VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
            case PIPELINE_STAGE_PIXEL:       return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            case PIPELINE_STAGE_COMPUTE:     return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            default: return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        }
    }


	inline VkDescriptorType ConvertResourceToDescriptorType(ResourceType type, Usage usage)
	{
		switch (type)
		{
			case RESOURCE_TYPE_TEXTURE:			return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            case RESOURCE_TYPE_CONSTANT_BUFFER: return usage == USAGE_DYNAMIC ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case RESOURCE_TYPE_SAMPLER_STATE:	return VK_DESCRIPTOR_TYPE_SAMPLER;
            default:                            return VK_DESCRIPTOR_TYPE_MAX_ENUM;
		}
	}


	inline uint32 ConvertShaderStages(uint32 stages)
	{
		uint32 vkStages = 0;
		if (stages & SHADER_STAGE_VERTEX)
			vkStages |= VK_SHADER_STAGE_VERTEX_BIT;
		if (stages & SHADER_STAGE_HULL)
			vkStages |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		if (stages & SHADER_STAGE_DOMAIN)
			vkStages |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		if (stages & SHADER_STAGE_GEOMETRY)
			vkStages |= VK_SHADER_STAGE_GEOMETRY_BIT;
		if (stages & SHADER_STAGE_PIXEL)
			vkStages |= VK_SHADER_STAGE_FRAGMENT_BIT;
		if (stages & SHADER_STAGE_COMPUTE)
			vkStages |= VK_SHADER_STAGE_COMPUTE_BIT;

		return vkStages;
	}
}
