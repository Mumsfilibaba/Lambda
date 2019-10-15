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
		//Release all framebuffers with this imageview
		VKNFramebufferCache& cache = VKNFramebufferCache::Get();
		cache.OnReleaseImageView(m_Resource);

		//Release resource
		vkDestroyImageView(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkRenderPass>::DestroyResource()
	{
		//Release all framebuffers with this renderpass
		VKNFramebufferCache& cache = VKNFramebufferCache::Get();
		cache.OnReleaseRenderPass(m_Resource);

		//Release resource
		vkDestroyRenderPass(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkDescriptorPool>::DestroyResource()
	{
		vkDestroyDescriptorPool(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkPipelineLayout>::DestroyResource()
	{
		vkDestroyPipelineLayout(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkPipeline>::DestroyResource()
	{
		vkDestroyPipeline(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkDescriptorSetLayout>::DestroyResource()
	{
		vkDestroyDescriptorSetLayout(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkQueryPool>::DestroyResource()
	{
		vkDestroyQueryPool(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkSampler>::DestroyResource()
	{
		vkDestroySampler(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;
	}
	

	template<>
	void VKNResource<VkShaderModule>::DestroyResource()
	{
		vkDestroyShaderModule(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkImage>::DestroyResource()
	{
		vkDestroyImage(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkSemaphore>::DestroyResource()
	{
		vkDestroySemaphore(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkBuffer>::DestroyResource()
	{
		vkDestroyBuffer(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkFramebuffer>::DestroyResource()
	{
		vkDestroyFramebuffer(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;
	}


	template<>
	void VKNResource<VkFence>::DestroyResource()
	{
		vkDestroyFence(m_pDevice->GetVkDevice(), m_Resource, nullptr);
		m_Resource = VK_NULL_HANDLE;
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
		m_FrameIndex = (m_FrameIndex + 1) % numFrames;
		
		//Erase resources
		for (auto i = m_Resources.begin(); i != m_Resources.end();)
		{
			if (i->first == m_FrameIndex)
				i = m_Resources.erase(i);
			else
				i++;
			
			LOG_DEBUG_INFO("Deleting\n");
		}
	}
}