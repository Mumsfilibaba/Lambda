#include "LambdaPch.h"
#include "VulkanFramebuffer.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanTexture2D.h"

namespace Lambda
{
	//Static variable declarations
	std::unordered_map<VkImageView, VulkanFramebufferCacheKey> VulkanFramebufferCache::s_Framebuffers = std::unordered_map<VkImageView, VulkanFramebufferCacheKey>();


	//VulkanFramebufferCacheKey
	VulkanFramebufferCacheKey::VulkanFramebufferCacheKey()
		: RenderPass(VK_NULL_HANDLE),
		FrameBuffer(VK_NULL_HANDLE),
		NumColorAttachments(0),
		DepthStencilView(nullptr),
		ColorAttachmentViews()
	{
		for (uint32 i = 0; i < LAMBDA_MAX_RENDERTARGET_COUNT; i++)
			ColorAttachmentViews[i] = VK_NULL_HANDLE;
	}


	bool VulkanFramebufferCacheKey::operator==(const VulkanFramebufferCacheKey& other)
	{
		if (RenderPass != other.RenderPass ||
			NumColorAttachments != other.NumColorAttachments ||
			DepthStencilView != other.DepthStencilView)
		{
			return false;
		}

		for (uint32 i = 0; i < NumColorAttachments; i++)
		{
			if (ColorAttachmentViews[i] != other.ColorAttachmentViews[i])
			{
				return false;
			}
		}

		return true;
	}


	bool VulkanFramebufferCacheKey::ContainsTexture(const VulkanTexture2D* pTexture)
	{
		VkImageView view = pTexture->GetImageView();
		if (DepthStencilView == view)
		{
			return true;
		}

		for (uint32 i = 0; i < NumColorAttachments; i++)
		{
			if (ColorAttachmentViews[i] == view)
			{
				return true;
			}
		}

		return false;
	}


	//VulkanFramebufferCache
	VkFramebuffer VulkanFramebufferCache::GetFramebuffer(VkDevice device, const VulkanFramebufferCacheKey& fbKey, uint32 width, uint32 height)
	{
		assert(device != VK_NULL_HANDLE);

		//Get key
		VkImageView key = VK_NULL_HANDLE;
		if (fbKey.NumColorAttachments > 0)
		{
			key = fbKey.ColorAttachmentViews[0];
		}
		else
		{
			key = fbKey.DepthStencilView;
		}


		//If the key is null return
		if (key == VK_NULL_HANDLE)
		{
			return VK_NULL_HANDLE;
		}


		//Check if a framebuffer exists
		auto range = s_Framebuffers.equal_range(key);
		for (auto i = range.first; i != range.second; i++)
		{
			//Find the matching framebuffer
			auto& value = i->second;
			if (value == fbKey)
				return value.FrameBuffer;
		}


		//Add all attachments
		std::vector<VkImageView> attachments(fbKey.ColorAttachmentViews, fbKey.ColorAttachmentViews + fbKey.NumColorAttachments);
		if (fbKey.DepthStencilView != VK_NULL_HANDLE)
		{
			attachments.push_back(fbKey.DepthStencilView);
		}


		//Setup new framebuffer
		VkFramebufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.pNext = nullptr;
		info.renderPass = fbKey.RenderPass;
		info.attachmentCount = uint32(attachments.size());
		info.pAttachments = attachments.data();
		info.width = width;
		info.height = height;
		info.layers = 1;

		//Create new framebuffer
		VkFramebuffer framebuffer = VK_NULL_HANDLE;
		if (vkCreateFramebuffer(device, &info, nullptr, &framebuffer) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create Framebuffer\n");
			return VK_NULL_HANDLE;
		}
		else
		{
			LOG_DEBUG_INFO("Vulkan: Created new Framebuffer\n");

			//Insert new framebuffer
			VulkanFramebufferCacheKey newKey = {};
			memcpy(&newKey, &fbKey, sizeof(VulkanFramebufferCacheKey));

			newKey.FrameBuffer = framebuffer;

			s_Framebuffers.insert(std::pair<VkImageView, VulkanFramebufferCacheKey>(key, newKey));
			return framebuffer;
		}
	}


	void VulkanFramebufferCache::ReleaseAllContainingTexture(VkDevice device, const VulkanTexture2D* pTexture)
	{
		//Find all framebuffers containing this texture
		auto range = s_Framebuffers.equal_range(pTexture->GetImageView());
		for (auto i = range.first; i != range.second; i++)
		{
			if (i->second.ContainsTexture(pTexture))
			{
				vkDestroyFramebuffer(device, i->second.FrameBuffer, nullptr);
				i->second.FrameBuffer = VK_NULL_HANDLE;
			}
		}

		//Erase the range
		s_Framebuffers.erase(range.first, range.second);
	}


	void VulkanFramebufferCache::ReleaseAll(VkDevice device)
	{
		//Destroy all framebuffers
		for (auto& buffer : s_Framebuffers)
		{

			vkDestroyFramebuffer(device, buffer.second.FrameBuffer, nullptr);
			buffer.second.FrameBuffer = VK_NULL_HANDLE;
		}

		s_Framebuffers.clear();
	}
}