#pragma once
#include "LambdaPch.h"
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

		bool operator==(const VulkanFramebufferCacheKey& other);
		bool ContainsTexture(const VulkanTexture* pTexture);

	public:
        VkRenderPass RenderPass;
        VkFramebuffer FrameBuffer;
        uint32 NumColorAttachments;
        VkImageView DepthStencilView;
        VkImageView ColorAttachmentViews[LAMBDA_MAX_RENDERTARGET_COUNT];
    };
    
    
    //Stores all framebuffers in the program
    class VulkanFramebufferCache final
    {
    public:
        LAMBDA_STATIC_CLASS(VulkanFramebufferCache);
        
        static VkFramebuffer GetFramebuffer(VkDevice device, const VulkanFramebufferCacheKey& key, uint32 width, uint32 height);
        static void ReleaseAllContainingTexture(VkDevice device, const VulkanTexture* pTexture);
        static void ReleaseAll(VkDevice device);
        
    private:
        static std::unordered_map<VkImageView, VulkanFramebufferCacheKey> s_Framebuffers;
    };
}
