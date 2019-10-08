#include "LambdaPch.h"
#include "VKNFramebuffer.h"
#include "VKNDevice.h"
#include "VKNTexture.h"
#include "Utilities/HashHelper.h"

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


	bool VKNFramebufferCacheKey::ContainsTexture(const VKNTexture* pTexture) const
	{
		VkImageView view = pTexture->GetVkImageView();
		for (uint32 i = 0; i < NumAttachmentViews; i++)
		{
			if (AttachmentViews[i] == view)
				return true;
		}

		return false;
	}

	//-------------------
	//VKNFramebufferCache
	//-------------------

	VKNFramebufferCache* VKNFramebufferCache::s_pInstance = nullptr;

	VKNFramebufferCache::VKNFramebufferCache()
		: m_Framebuffers()
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
		
		VKNDevice& device = VKNDevice::Get();
		if (vkCreateFramebuffer(device.GetVkDevice(), &info, nullptr, &framebuffer) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create Framebuffer\n");
			return VK_NULL_HANDLE;
		}
		else
		{
			LOG_SYSTEM_PRINT("Vulkan: Created new Framebuffer\n");

			m_Framebuffers.insert(std::pair<VKNFramebufferCacheKey, VkFramebuffer>(fbKey, framebuffer));
			return framebuffer;
		}
	}


	void VKNFramebufferCache::ReleaseAllContainingTexture(VkDevice device, const VKNTexture* pTexture)
	{
		//Find all framebuffers containing this texture
		for (auto it = m_Framebuffers.begin(); it != m_Framebuffers.end();)
		{
			if (it->first.ContainsTexture(pTexture))
			{
				//Destroy framebuffer
				vkDestroyFramebuffer(device, it->second, nullptr);
				it->second = VK_NULL_HANDLE;

				it = m_Framebuffers.erase(it);
			}
            else
            {
                it++;
            }
		}
	}


	void VKNFramebufferCache::ReleaseAll(VkDevice device)
	{
		//Destroy all framebuffers
		for (auto& buffer : m_Framebuffers)
		{
			VkFramebuffer& fb = buffer.second;
			vkDestroyFramebuffer(device, fb, nullptr);
			fb = VK_NULL_HANDLE;
		}

		m_Framebuffers.clear();
	}
	
	
	VKNFramebufferCache& VKNFramebufferCache::Get()
	{
		LAMBDA_ASSERT(s_pInstance != nullptr);
		return *s_pInstance;
	}
}
