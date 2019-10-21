#include "LambdaPch.h"
#include "VKNDevice.h"
#include "VKNFramebufferCache.h"
#include "VKNSafeReleaseManager.h"

//#define LAMBDA_SAFE_RESOURCE_DEBUG

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

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkImageView\n");
#endif
	}


	template<>
	void VKNResource<VkRenderPass>::DestroyResource()
	{
		//Release resource
		vkDestroyRenderPass(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkRenderPass\n");
#endif
	}


	template<>
	void VKNResource<VkDescriptorPool>::DestroyResource()
	{
		vkDestroyDescriptorPool(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkDescriptorPool\n");
#endif
	}


	template<>
	void VKNResource<VkPipelineLayout>::DestroyResource()
	{
		vkDestroyPipelineLayout(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkPipelineLayout\n");
#endif
	}


	template<>
	void VKNResource<VkPipeline>::DestroyResource()
	{
		vkDestroyPipeline(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkPipeline\n");
#endif
	}


	template<>
	void VKNResource<VkDescriptorSetLayout>::DestroyResource()
	{
		vkDestroyDescriptorSetLayout(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkDescriptorSetLayout\n");
#endif
	}


	template<>
	void VKNResource<VkQueryPool>::DestroyResource()
	{
		vkDestroyQueryPool(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkQueryPool\n");
#endif
	}


	template<>
	void VKNResource<VkSampler>::DestroyResource()
	{
		vkDestroySampler(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkSampler\n");
#endif
	}
	

	template<>
	void VKNResource<VkShaderModule>::DestroyResource()
	{
		vkDestroyShaderModule(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkShaderModule\n");
#endif
	}


	template<>
	void VKNResource<VkImage>::DestroyResource()
	{
		vkDestroyImage(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkImage\n");
#endif
	}


	template<>
	void VKNResource<VkSemaphore>::DestroyResource()
	{
		vkDestroySemaphore(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkSemaphore\n");
#endif
	}


	template<>
	void VKNResource<VkBuffer>::DestroyResource()
	{
		vkDestroyBuffer(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkBuffer\n");
#endif
	}


	template<>
	void VKNResource<VkFramebuffer>::DestroyResource()
	{
		vkDestroyFramebuffer(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkFramebuffer\n");
#endif
	}


	template<>
	void VKNResource<VkFence>::DestroyResource()
	{
		vkDestroyFence(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkFence\n");
#endif
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
			uint64 frame = it->first;
			if (frame == m_FrameIndex)
			{
				//Release
				if (it->second)
					it->second->Release();

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