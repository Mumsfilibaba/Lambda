#pragma once
#include <vulkan/vulkan.h>
#include <unordered_map>

namespace Lambda
{
    class VulkanTexture;
    
    
    struct VulkanFramebufferCacheKey
    {
	public:
		VulkanFramebufferCacheKey();
		~VulkanFramebufferCacheKey() = default;

		size_t GetHash() const;
		bool ContainsTexture(const VulkanTexture* pTexture) const;

		bool operator==(const VulkanFramebufferCacheKey& other) const;

	public:
		mutable size_t Hash;
        VkRenderPass RenderPass;
        uint32 NumAttachmentViews;
        VkImageView AttachmentViews[(LAMBDA_MAX_RENDERTARGET_COUNT + 1) * 2];
    };


    class VulkanFramebufferCache final
    {
    public:
        LAMBDA_STATIC_CLASS(VulkanFramebufferCache);
        
        static VkFramebuffer GetFramebuffer(VkDevice device, const VulkanFramebufferCacheKey& key, uint32 width, uint32 height);
        static void ReleaseAllContainingTexture(VkDevice device, const VulkanTexture* pTexture);
        static void ReleaseAll(VkDevice device);
        
    private:
        static std::unordered_map<VulkanFramebufferCacheKey, VkFramebuffer> s_Framebuffers;
    };
}
