#include "LambdaPch.h"
#include "VKNSamplerState.h"
#include "VKNDevice.h"
#include "VKNConversions.inl"

namespace Lambda
{
	//---------------
	//VKNSamplerState
	//---------------

    VKNSamplerState::VKNSamplerState(const SamplerStateDesc& desc)
        : m_Sampler(VK_NULL_HANDLE),
        m_Desc()
    {
        Init(desc);
    }
    
    
	SamplerStateDesc VKNSamplerState::GetDesc() const
    {
        return m_Desc;
    }
    
    
    void* VKNSamplerState::GetNativeHandle() const
    {
        return reinterpret_cast<VkSampler>(m_Sampler);
    }
    
    
    void VKNSamplerState::Init(const SamplerStateDesc& desc)
    {
        //Get adress mode
        VkSamplerAddressMode adressMode = ConvertSamplerAdressMode(desc.AdressMode);
        
		//Create sampler
        VkSamplerCreateInfo info = {};
        info.sType                      = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        info.pNext                      = nullptr;
        info.flags                      = 0;
        info.magFilter                  = VK_FILTER_LINEAR;
        info.minFilter                  = VK_FILTER_LINEAR;
        info.addressModeU               = adressMode;
        info.addressModeV               = adressMode;
        info.addressModeW               = adressMode;
        info.anisotropyEnable           = VK_TRUE;
        info.maxAnisotropy              = 16;
        info.borderColor                = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        info.unnormalizedCoordinates    = VK_FALSE;
        info.compareEnable              = VK_FALSE;
        info.compareOp                  = VK_COMPARE_OP_ALWAYS;
        info.mipmapMode                 = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        info.minLod                     = desc.MinMipLOD;
        info.maxLod                     = desc.MaxMipLOD;
        info.mipLodBias                 = desc.MipLODBias;
        
		VKNDevice& device = VKNDevice::GetInstance();
        if (vkCreateSampler(device.GetDevice(), &info, nullptr, &m_Sampler) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create samplerstate\n");
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created samplerstate\n");
        }
    }
    
    
    void VKNSamplerState::Destroy(VkDevice device)
    {
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);
        
        if (m_Sampler != VK_NULL_HANDLE)
        {
            vkDestroySampler(device, m_Sampler, nullptr);
            m_Sampler = VK_NULL_HANDLE;
        }

		delete this;
    }
}