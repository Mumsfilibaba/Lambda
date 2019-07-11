#include "Types.h"

namespace Lambda
{
    inline VkFormat ConvertResourceFormat(ResourceFormat format)
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
    
    
    inline ResourceFormat ConvertVkFormat(VkFormat format)
    {
        switch (format)
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
            case PRIMITIVE_TOPOLOGY_TRIANGLELIST: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            case PRIMITIVE_TOPOLOGY_TRIANGLESTRIP: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            default: return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
        }
    }
    
    
    inline PrimitiveTopology ConvertVkPrimitiveTopology(VkPrimitiveTopology toplogy)
    {
        switch (toplogy)
        {
            case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: return PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: return PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            default: return PRIMITIVE_TOPOLOGY_UNKNOWN;
        }
    }
    
    
    inline VkImageLayout ConvertResourceStateToImageLayout(ResourceState state)
    {
        switch (state)
        {
            case RESOURCE_STATE_PIXEL_SHADER_RESOURCE:  return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            case RESOURCE_STATE_COPY_DEST:              return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            case RESOURCE_STATE_UNKNOWN:
            default: return VK_IMAGE_LAYOUT_UNDEFINED;
        }
    }
    
    
    inline VkSamplerAddressMode ConvertSamplerAdressMode(SamplerAddressMode adressMode)
    {
        switch (adressMode) {
            case SAMPLER_ADDRESS_MODE_REPEAT:               return VK_SAMPLER_ADDRESS_MODE_REPEAT;
            case SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT:      return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
            case SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE:        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            case SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER:      return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            case SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
            case SAMPLER_ADDRESS_MODE_UNKNOWN:
            default: return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
        }
    }
}
