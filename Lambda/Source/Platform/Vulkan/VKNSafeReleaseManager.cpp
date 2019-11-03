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

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkImageView '%p'\n", m_Resource);
#endif

		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkRenderPass>::DestroyResource()
	{
		//Release resource
		vkDestroyRenderPass(m_pDevice->GetVkDevice(), m_Resource, nullptr);

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkRenderPass '%p'\n", m_Resource);
#endif

		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkDescriptorPool>::DestroyResource()
	{
		vkDestroyDescriptorPool(m_pDevice->GetVkDevice(), m_Resource, nullptr);

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkDescriptorPool '%p'\n", m_Resource);
#endif

		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkPipelineLayout>::DestroyResource()
	{
		vkDestroyPipelineLayout(m_pDevice->GetVkDevice(), m_Resource, nullptr);

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkPipelineLayout '%p'\n", m_Resource);
#endif

		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkPipeline>::DestroyResource()
	{
		vkDestroyPipeline(m_pDevice->GetVkDevice(), m_Resource, nullptr);

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkPipeline '%p'\n", m_Resource);
#endif

		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkDescriptorSetLayout>::DestroyResource()
	{
		vkDestroyDescriptorSetLayout(m_pDevice->GetVkDevice(), m_Resource, nullptr);

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkDescriptorSetLayout '%p'\n", m_Resource);
#endif

		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkQueryPool>::DestroyResource()
	{
		vkDestroyQueryPool(m_pDevice->GetVkDevice(), m_Resource, nullptr);

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkQueryPool '%p'\n", m_Resource);
#endif

		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkSampler>::DestroyResource()
	{
		vkDestroySampler(m_pDevice->GetVkDevice(), m_Resource, nullptr);

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkSampler '%p'\n", m_Resource);
#endif

		m_Resource = VK_NULL_HANDLE;
	}
	

	template<>
	void VKNResource<VkShaderModule>::DestroyResource()
	{
		vkDestroyShaderModule(m_pDevice->GetVkDevice(), m_Resource, nullptr);

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkShaderModule '%p'\n", m_Resource);
#endif

		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkImage>::DestroyResource()
	{
		vkDestroyImage(m_pDevice->GetVkDevice(), m_Resource, nullptr);

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkImage '%p'\n", m_Resource);
#endif

		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkSemaphore>::DestroyResource()
	{
		vkDestroySemaphore(m_pDevice->GetVkDevice(), m_Resource, nullptr);

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkSemaphore '%p'\n", m_Resource);
#endif

		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkBuffer>::DestroyResource()
	{
		vkDestroyBuffer(m_pDevice->GetVkDevice(), m_Resource, nullptr);

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkBuffer '%p'\n", m_Resource);
#endif

		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkFramebuffer>::DestroyResource()
	{
		vkDestroyFramebuffer(m_pDevice->GetVkDevice(), m_Resource, nullptr);

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkFramebuffer '%p'\n", m_Resource);
#endif

		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkFence>::DestroyResource()
	{
		vkDestroyFence(m_pDevice->GetVkDevice(), m_Resource, nullptr);

#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
		LOG_DEBUG_INFO("Releasing VkFence '%p'\n", m_Resource);
#endif

		m_Resource = VK_NULL_HANDLE;
	}


	//---------------------
	//VKNSafeReleaseManager
	//---------------------
	
	constexpr size_t numFrames = 4;

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
				{
#if defined(LAMBDA_SAFE_RESOURCE_DEBUG)
				LOG_DEBUG_WARNING("Deleting VkResource '%p' Frame Added=%llu, Frame Now=%llu\n", it->second->GetResourceHandle(), frame, m_FrameIndex);
#endif

					it->second->Release();
				}

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