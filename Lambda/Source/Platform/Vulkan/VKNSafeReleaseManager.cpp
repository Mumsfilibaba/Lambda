#include "LambdaPch.h"
#include "VKNDevice.h"
#include "VKNFramebufferCache.h"
#include "VKNSafeReleaseManager.h"

namespace Lambda
{
	//-----------
	//VKNResource
	//-----------
	
	template<>
	void VKNResource<VkImageView>::DestroyResource()
	{
		//Release resource
		vkDestroyImageView(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

		LOG_DEBUG_INFO("Releasing VkImageView\n");
	}


	template<>
	void VKNResource<VkRenderPass>::DestroyResource()
	{
		//Release resource
		vkDestroyRenderPass(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

		LOG_DEBUG_INFO("Releasing VkRenderPass\n");
	}


	template<>
	void VKNResource<VkDescriptorPool>::DestroyResource()
	{
		vkDestroyDescriptorPool(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

		LOG_DEBUG_INFO("Releasing VkDescriptorPool\n");
	}


	template<>
	void VKNResource<VkPipelineLayout>::DestroyResource()
	{
		vkDestroyPipelineLayout(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

		LOG_DEBUG_INFO("Releasing VkPipelineLayout\n");
	}


	template<>
	void VKNResource<VkPipeline>::DestroyResource()
	{
		vkDestroyPipeline(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

		LOG_DEBUG_INFO("Releasing VkPipeline\n");
	}


	template<>
	void VKNResource<VkDescriptorSetLayout>::DestroyResource()
	{
		vkDestroyDescriptorSetLayout(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

		LOG_DEBUG_INFO("Releasing VkDescriptorSetLayout\n");
	}


	template<>
	void VKNResource<VkQueryPool>::DestroyResource()
	{
		vkDestroyQueryPool(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

		LOG_DEBUG_INFO("Releasing VkQueryPool\n");
	}


	template<>
	void VKNResource<VkSampler>::DestroyResource()
	{
		vkDestroySampler(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

		LOG_DEBUG_INFO("Releasing VkSampler\n");
	}
	

	template<>
	void VKNResource<VkShaderModule>::DestroyResource()
	{
		vkDestroyShaderModule(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

		LOG_DEBUG_INFO("Releasing VkShaderModule\n");
	}


	template<>
	void VKNResource<VkImage>::DestroyResource()
	{
		vkDestroyImage(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

		LOG_DEBUG_INFO("Releasing VkImage\n");
	}


	template<>
	void VKNResource<VkSemaphore>::DestroyResource()
	{
		vkDestroySemaphore(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

		LOG_DEBUG_INFO("Releasing VkSemaphore\n");
	}


	template<>
	void VKNResource<VkBuffer>::DestroyResource()
	{
		vkDestroyBuffer(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

		LOG_DEBUG_INFO("Releasing VkBuffer\n");
	}


	template<>
	void VKNResource<VkFramebuffer>::DestroyResource()
	{
		vkDestroyFramebuffer(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

		LOG_DEBUG_INFO("Releasing VkFramebuffer\n");
	}


	template<>
	void VKNResource<VkFence>::DestroyResource()
	{
		vkDestroyFence(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

		LOG_DEBUG_INFO("Releasing VkFence\n");
	}


	//---------------------
	//VKNSafeReleaseManager
	//---------------------
	
	constexpr size_t numFrames = 5;

	VKNSafeReleaseManager::VKNSafeReleaseManager(VKNDevice* pDevice)
		: m_pDevice(pDevice),
		m_FrameIndex(0),
		m_Resources()
	{
	}

	
	VKNSafeReleaseManager::~VKNSafeReleaseManager()
	{
		//Release all resources
		for (size_t i = 0; i < numFrames; i++)
			EmptyResources();
	}

	
	void VKNSafeReleaseManager::EmptyResources()
	{
		//Increase frameindex
		m_FrameIndex = (m_FrameIndex+1) % numFrames;
		for (auto it = m_Resources.begin(); it != m_Resources.end();)
		{
			uint32 frame = it->first;
			if (frame == m_FrameIndex)
			{
				//Erase from vector
				it = m_Resources.erase(it);
			}
			else
			{
				it++;
			}
		}
	}
}