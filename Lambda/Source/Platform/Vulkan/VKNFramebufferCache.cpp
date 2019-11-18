#include "LambdaPch.h"
#include "Utilities/HashHelper.h"
#include "VKNFramebufferCache.h"
#include "VKNDevice.h"
#include "VKNTexture.h"

namespace Lambda
{
	//----------------------
	//VKNFramebufferCacheKey
	//----------------------

	VKNFramebufferCacheKey::VKNFramebufferCacheKey()
		: Hash(0),
		RenderPass(VK_NULL_HANDLE),
		NumAttachmentViews(0),
		AttachmentViews()
	{
		for (uint32 i = 0; i < (LAMBDA_MAX_RENDERTARGET_COUNT + 1); i++)
			AttachmentViews[i] = VK_NULL_HANDLE;
	}


	bool VKNFramebufferCacheKey::operator==(const VKNFramebufferCacheKey& other) const
	{
		if (GetHash() != other.GetHash() || RenderPass != other.RenderPass || NumAttachmentViews != other.NumAttachmentViews)
			return false;

		for (uint32 i = 0; i < NumAttachmentViews; i++)
		{
			if (AttachmentViews[i] != other.AttachmentViews[i])
				return false;
		}

		return true;
	}


	std::size_t VKNFramebufferCacheKey::GetHash() const
	{
		if (Hash == 0)
		{
			HashCombine<VkRenderPass>(Hash, RenderPass);
			for (uint32 i = 0; i < NumAttachmentViews; i++)
				HashCombine<VkImageView>(Hash, AttachmentViews[i]);
		}

		return Hash;
	}


	bool VKNFramebufferCacheKey::ContainsImageView(VkImageView view) const
	{
		for (uint32 i = 0; i < NumAttachmentViews; i++)
		{
			if (AttachmentViews[i] == view)
				return true;
		}

		return false;
	}


	bool VKNFramebufferCacheKey::ContainsRenderPass(VkRenderPass renderpass) const
	{
		return renderpass == RenderPass;
	}

	//-------------------
	//VKNFramebufferCache
	//-------------------

	VKNFramebufferCache* VKNFramebufferCache::s_pInstance = nullptr;

	VKNFramebufferCache::VKNFramebufferCache(VKNDevice* pDevice)
		: m_pDevice(pDevice),
		m_Framebuffers()
	{
		LAMBDA_ASSERT(s_pInstance == nullptr);
		s_pInstance = this;
	}

	
	VKNFramebufferCache::~VKNFramebufferCache()
	{
		if (s_pInstance == this)
			s_pInstance = nullptr;
	}


	VkFramebuffer VKNFramebufferCache::GetFramebuffer(const VKNFramebufferCacheKey& fbKey, VkExtent2D extent)
	{
		//Check if a framebuffer exists
		auto fb = m_Framebuffers.find(fbKey);
		if (fb != m_Framebuffers.end())
		{
			return fb->second;
		}

		//Create new framebuffer
		VkFramebufferCreateInfo info = {};
		info.sType				= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.pNext				= nullptr;
		info.renderPass			= fbKey.RenderPass;
		info.attachmentCount	= fbKey.NumAttachmentViews;
		info.pAttachments		= fbKey.AttachmentViews;
		info.width				= extent.width;
		info.height				= extent.height;
		info.layers				= 1;

		VkFramebuffer framebuffer = VK_NULL_HANDLE;
		if (vkCreateFramebuffer(m_pDevice->GetVkDevice(), &info, nullptr, &framebuffer) != VK_SUCCESS)
		{
			LOG_RENDER_API_ERROR("[Vulkan] Failed to create Framebuffer\n");
			return VK_NULL_HANDLE;
		}
		else
		{
			LOG(LOG_CHANNEL_RENDER_API, LOG_SEVERITY_INFO, "[Vulkan] Created new Framebuffer\n");

			m_Framebuffers.insert(std::pair<VKNFramebufferCacheKey, VkFramebuffer>(fbKey, framebuffer));
			return framebuffer;
		}
	}


	void VKNFramebufferCache::OnReleaseImageView(VkImageView view)
	{
		//Find all framebuffers containing this texture
		for (auto it = m_Framebuffers.begin(); it != m_Framebuffers.end();)
		{
			if (it->first.ContainsImageView(view))
			{
				//Destroy framebuffer
				if (it->second != VK_NULL_HANDLE)
				{
					m_pDevice->SafeReleaseVkResource<VkFramebuffer>(it->second);
					it->second = VK_NULL_HANDLE;
				}

				//Erase from vector
				it = m_Framebuffers.erase(it);
			}
            else
            {
                it++;
            }
		}
	}


	void VKNFramebufferCache::OnReleaseRenderPass(VkRenderPass renderpass)
	{
		//Find all framebuffers containing this renderpass
		for (auto it = m_Framebuffers.begin(); it != m_Framebuffers.end();)
		{
			if (it->first.ContainsRenderPass(renderpass))
			{
				//Destroy framebuffer
				if (it->second != VK_NULL_HANDLE)
				{
					m_pDevice->SafeReleaseVkResource<VkFramebuffer>(it->second);
					it->second = VK_NULL_HANDLE;
				}

				//Erase from vector
				it = m_Framebuffers.erase(it);
			}
			else
			{
				it++;
			}
		}
	}


	void VKNFramebufferCache::ReleaseAll()
	{
		//Destroy all framebuffers
		for (auto& buffer : m_Framebuffers)
		{
			if (buffer.second != VK_NULL_HANDLE)
				m_pDevice->SafeReleaseVkResource<VkFramebuffer>(buffer.second);
		}

		m_Framebuffers.clear();
	}
	
	
	VKNFramebufferCache& VKNFramebufferCache::Get()
	{
		LAMBDA_ASSERT(s_pInstance != nullptr);
		return *s_pInstance;
	}
}
