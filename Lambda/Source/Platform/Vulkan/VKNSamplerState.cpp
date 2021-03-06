#include "LambdaPch.h"
#include "VKNSamplerState.h"
#include "VKNDevice.h"
#include "VKNConversions.inl"

namespace Lambda
{
	//---------------
	//VKNSamplerState
	//---------------

	VKNSamplerState::VKNSamplerState(VKNDevice* pVkDevice, const SamplerStateDesc& desc)
		: TSamplerState(pVkDevice, desc),
		m_VkSampler(VK_NULL_HANDLE)
	{
		//Add a ref to the refcounter
		this->AddRef();
		Init(desc);
	}


	VKNSamplerState::~VKNSamplerState()
	{
		if (m_VkSampler != VK_NULL_HANDLE)
			m_pDevice->SafeReleaseVkResource<VkSampler>(m_VkSampler);

		LOG_DEBUG_INFO("[Vulkan] Destroyed SamplerState\n");
	}


	void VKNSamplerState::Init(const SamplerStateDesc& desc)
	{
		//Get adress mode
		VkSamplerAddressMode adressMode = ConvertSamplerAdressMode(desc.AdressMode);

		//Create sampler
		VkSamplerCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.magFilter = VK_FILTER_LINEAR;
		info.minFilter = VK_FILTER_LINEAR;
		info.addressModeU = adressMode;
		info.addressModeV = adressMode;
		info.addressModeW = adressMode;
		info.anisotropyEnable = VK_TRUE;
		info.maxAnisotropy = desc.Anisotropy;
		info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		info.unnormalizedCoordinates = VK_FALSE;
		info.compareEnable = VK_FALSE;
		info.compareOp = VK_COMPARE_OP_ALWAYS;
		info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		info.minLod = desc.MinMipLOD;
		info.maxLod = desc.MaxMipLOD;
		info.mipLodBias = desc.MipLODBias;
		if (vkCreateSampler(m_pDevice->GetVkDevice(), &info, nullptr, &m_VkSampler) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("[Vulkan] Failed to create samplerstate\n");
		}
		else
		{
			LOG_DEBUG_INFO("[Vulkan] Created samplerstate\n");
			SetName(desc.pName);
		}
	}


	void* VKNSamplerState::GetNativeHandle() const
	{
		return reinterpret_cast<VkSampler>(m_VkSampler);
	}
	
	
	void VKNSamplerState::SetName(const char* pName)
	{
		TSamplerState::SetName(pName);
		m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_SAMPLER, (uint64)m_VkSampler, m_Name);
		m_Desc.pName = m_Name.c_str();
	}
}
