#include "LambdaPch.h"
#include "VulkanFramebuffer.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanTexture.h"

namespace Lambda
{
	std::unordered_map<VulkanFramebufferCacheKey, VkFramebuffer, VulkanFramebufferCacheKeyHash> VulkanFramebufferCache::s_Framebuffers = std::unordered_map<VulkanFramebufferCacheKey, VkFramebuffer, VulkanFramebufferCacheKeyHash>();


    template<typename T>
    inline void HashCombine(size_t& hash, const T& value)
    {
        std::hash<T> hasher;
        hash ^= hasher(value) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    
    
	VulkanFramebufferCacheKey::VulkanFramebufferCacheKey()
		: Hash(0),
		RenderPass(VK_NULL_HANDLE),
		NumAttachmentViews(0),
		AttachmentViews()
	{
		for (uint32 i = 0; i < (LAMBDA_MAX_RENDERTARGET_COUNT + 1) * 2; i++)
			AttachmentViews[i] = VK_NULL_HANDLE;
	}


	bool VulkanFramebufferCacheKey::operator==(const VulkanFramebufferCacheKey& other) const
	{
		if (RenderPass != other.RenderPass ||
			NumAttachmentViews != other.NumAttachmentViews)
		{
			return false;
		}

		for (uint32 i = 0; i < NumAttachmentViews; i++)
		{
			if (AttachmentViews[i] != other.AttachmentViews[i])
			{
				return false;
			}
		}

		return true;
	}


	std::size_t VulkanFramebufferCacheKey::GetHash() const
	{
		if (Hash == 0)
		{
			for (uint32 i = 0; i < NumAttachmentViews; i++)
			{
				HashCombine<VkImageView>(Hash, AttachmentViews[i]);
			}
		}

		return Hash;
	}


	bool VulkanFramebufferCacheKey::ContainsTexture(const VulkanTexture* pTexture) const
	{
		VkImageView view = pTexture->GetImageView();
		for (uint32 i = 0; i < NumAttachmentViews; i++)
		{
			if (AttachmentViews[i] == view)
			{
				return true;
			}
		}

		return false;
	}


	VkFramebuffer VulkanFramebufferCache::GetFramebuffer(VkDevice device, const VulkanFramebufferCacheKey& fbKey, uint32 width, uint32 height)
	{
		assert(device != VK_NULL_HANDLE);

		//Check if a framebuffer exists
		auto fb = s_Framebuffers.find(fbKey);
		if (fb != s_Framebuffers.end())
		{
			return fb->second;
		}

		//Create new framebuffer
		VkFramebufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.pNext = nullptr;
		info.renderPass = fbKey.RenderPass;
		info.attachmentCount = fbKey.NumAttachmentViews;
		info.pAttachments = fbKey.AttachmentViews;
		info.width = width;
		info.height = height;
		info.layers = 1;

		VkFramebuffer framebuffer = VK_NULL_HANDLE;
		if (vkCreateFramebuffer(device, &info, nullptr, &framebuffer) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create Framebuffer\n");
			return VK_NULL_HANDLE;
		}
		else
		{
			LOG_SYSTEM_PRINT("Vulkan: Created new Framebuffer\n");

			s_Framebuffers.insert(std::pair<VulkanFramebufferCacheKey, VkFramebuffer>(fbKey, framebuffer));
			return framebuffer;
		}
	}


	void VulkanFramebufferCache::ReleaseAllContainingTexture(VkDevice device, const VulkanTexture* pTexture)
	{
		//Find all framebuffers containing this texture
		for (auto it = s_Framebuffers.begin(); it != s_Framebuffers.end();)
		{
			if (it->first.ContainsTexture(pTexture))
			{
				//Destroy framebuffer
				vkDestroyFramebuffer(device, it->second, nullptr);
				it->second = VK_NULL_HANDLE;

				it = s_Framebuffers.erase(it);
			}
            else
            {
                it++;
            }
		}
	}


	void VulkanFramebufferCache::ReleaseAll(VkDevice device)
	{
		//Destroy all framebuffers
		for (auto& buffer : s_Framebuffers)
		{
			VkFramebuffer fb = buffer.second;
			vkDestroyFramebuffer(device, fb, nullptr);
			fb = VK_NULL_HANDLE;
		}

		s_Framebuffers.clear();
	}
}
