#include "LambdaPch.h"
#include "VKNDescriptorAllocator.h"
#include "../VKNDevice.h"

namespace Lambda
{
	//-------------------------
	//VKNDescriptorSetAllocator
	//-------------------------

	VKNDescriptorSetAllocator::VKNDescriptorSetAllocator(VKNDevice* pDevice, uint32 uniformBufferCount, uint32 dynamicUniformBufferCount, uint32 samplerCount, uint32 sampledImageCount, uint32 combinedImageSamplerCount, uint32 numSets)
		: m_pDevice(pDevice),
		m_Pool(VK_NULL_HANDLE),
		m_NumSets(0),
		m_UniformBufferCount(uniformBufferCount),
		m_DynamicUniformBufferCount(dynamicUniformBufferCount),
		m_SamplerCount(samplerCount),
		m_SampledImageCount(sampledImageCount),
		m_CombinedImageSamplerCount(combinedImageSamplerCount),
		m_SetCount(numSets)
	{
		Init();
	}


	VkDescriptorSet VKNDescriptorSetAllocator::Allocate(VkDescriptorSetLayout descriptorSetLayout)
	{
		//Check for more descriptor
		if (m_NumSets < 1)
			Init();

		VkDescriptorSet set = VK_NULL_HANDLE;

		//Allocate descriptorsets
		VkDescriptorSetAllocateInfo descriptorAllocInfo = {};
		descriptorAllocInfo.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorAllocInfo.pNext				= nullptr;
		descriptorAllocInfo.descriptorPool		= m_Pool;
		descriptorAllocInfo.descriptorSetCount	= 1;
		descriptorAllocInfo.pSetLayouts			= &descriptorSetLayout;
		if (vkAllocateDescriptorSets(m_pDevice->GetVkDevice(), &descriptorAllocInfo, &set))
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to allocate DescriptorSets\n");
			return VK_NULL_HANDLE;
		}
		else
		{
			LOG_DEBUG_INFO("Vulkan: Allocated DescriptorSets\n");
			m_NumSets--;

			return set;
		}
	}


	void VKNDescriptorSetAllocator::Destroy(VkDevice device)
	{
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);

		if (m_Pool != VK_NULL_HANDLE)
			m_pDevice->SafeReleaseVulkanResource<VkDescriptorPool>(m_Pool);

		delete this;
	}


	void VKNDescriptorSetAllocator::Init()
	{
		VkDescriptorPool pool = VK_NULL_HANDLE;

		//Setup descriptorcounts
		std::vector<VkDescriptorPoolSize> poolSizes;
		if (m_UniformBufferCount > 0)
		{
			VkDescriptorPoolSize poolSize = {};
			poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSize.descriptorCount = m_UniformBufferCount * m_SetCount;
			poolSizes.push_back(poolSize);
		}
		if (m_DynamicUniformBufferCount > 0)
		{
			VkDescriptorPoolSize poolSize = {};
			poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			poolSize.descriptorCount = m_DynamicUniformBufferCount * m_SetCount;
			poolSizes.push_back(poolSize);
		}
		if (m_SampledImageCount > 0)
		{
			VkDescriptorPoolSize poolSize = {};
			poolSize.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			poolSize.descriptorCount = m_SampledImageCount * m_SetCount;
			poolSizes.push_back(poolSize);
		}
		if (m_SamplerCount > 0)
		{
			VkDescriptorPoolSize poolSize = {};
			poolSize.type = VK_DESCRIPTOR_TYPE_SAMPLER;
			poolSize.descriptorCount = m_SamplerCount * m_SetCount;
			poolSizes.push_back(poolSize);
		}
		if (m_CombinedImageSamplerCount > 0)
		{
			VkDescriptorPoolSize poolSize = {};
			poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			poolSize.descriptorCount = m_CombinedImageSamplerCount * m_SetCount;
			poolSizes.push_back(poolSize);
		}

		//Create descriptorpool
		VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
		descriptorPoolInfo.sType		 = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.flags		 = 0;
		descriptorPoolInfo.pNext		 = nullptr;
		descriptorPoolInfo.poolSizeCount = uint32(poolSizes.size());
		descriptorPoolInfo.pPoolSizes	 = poolSizes.data();
		descriptorPoolInfo.maxSets		 = m_SetCount;
		if (vkCreateDescriptorPool(m_pDevice->GetVkDevice(), &descriptorPoolInfo, nullptr, &pool) != VK_SUCCESS)
		{
			if (m_Pool == VK_NULL_HANDLE)
			{
				LOG_DEBUG_ERROR("Vulkan: Failed to create DescriptorPool\n");
			}
			else
			{
				LOG_DEBUG_ERROR("Vulkan: Failed to reallocate DescriptorPool\n");
			}
		}
		else
		{
			if (m_Pool == VK_NULL_HANDLE)
			{
				LOG_DEBUG_INFO("Vulkan: Created DescriptorPool\n");
			}
			else
			{
				LOG_DEBUG_INFO("Vulkan: Reallocated DescriptorPool\n");
				m_pDevice->SafeReleaseVulkanResource<VkDescriptorPool>(m_Pool);
			}

			m_Pool = pool;
			m_NumSets = m_SetCount;
		}
	}
}