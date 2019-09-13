#pragma once
#include <vulkan/vulkan.h>
#include <unordered_map>

namespace Lambda
{
    class VulkanTexture;
    
	//-------------------------
	//VulkanFramebufferCacheKey
	//-------------------------

    struct VulkanFramebufferCacheKey
    {
	public:
		VulkanFramebufferCacheKey();
		~VulkanFramebufferCacheKey() = default;

		size_t	GetHash() const;
		bool	ContainsTexture(const VulkanTexture* pTexture) const;

		bool	operator==(const VulkanFramebufferCacheKey& other) const;

	public:
		mutable size_t	Hash;
        VkRenderPass	RenderPass;
        uint32			NumAttachmentViews;
        VkImageView		AttachmentViews[(LAMBDA_MAX_RENDERTARGET_COUNT + 1) * 2];
    };
    
	//-----------------------------
	//VulkanFramebufferCacheKeyHash
	//-----------------------------

    struct VulkanFramebufferCacheKeyHash
    {
        size_t operator()(const VulkanFramebufferCacheKey& key) const
        {
            return key.GetHash();
        }
    };

	//----------------------
	//VulkanFramebufferCache
	//----------------------

    class VulkanFramebufferCache final
    {
    public:
        LAMBDA_NO_COPY(VulkanFramebufferCache);
        
		VulkanFramebufferCache();
		~VulkanFramebufferCache();

        void			ReleaseAll(VkDevice device);
        void			ReleaseAllContainingTexture(VkDevice device, const VulkanTexture* pTexture);
        VkFramebuffer	GetFramebuffer(VkDevice device, const VulkanFramebufferCacheKey& key, uint32 width, uint32 height);
        
    private:
        std::unordered_map<VulkanFramebufferCacheKey, VkFramebuffer, VulkanFramebufferCacheKeyHash> m_Framebuffers;

	public:
		static VulkanFramebufferCache& GetInstance();

	private:
		static VulkanFramebufferCache* s_pInstance;
    };
}
