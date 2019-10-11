#pragma once
#include <vulkan/vulkan.h>
#include <unordered_map>

namespace Lambda
{
    class VKNTexture;
    
	//----------------------
	//VKNFramebufferCacheKey
	//----------------------

    struct VKNFramebufferCacheKey
    {
	public:
		VKNFramebufferCacheKey();
		~VKNFramebufferCacheKey() = default;

		size_t GetHash() const;
		bool ContainsTexture(const VKNTexture* pTexture) const;
		bool ContainsRenderPass(VkRenderPass renderpass) const;
		bool operator==(const VKNFramebufferCacheKey& other) const;
	public:
		mutable size_t	Hash;
        VkRenderPass	RenderPass;
        uint32			NumAttachmentViews;
        VkImageView		AttachmentViews[LAMBDA_MAX_RENDERTARGET_COUNT + 1];
    };
    
	//--------------------------
	//VKNFramebufferCacheKeyHash
	//--------------------------

    struct VKNFramebufferCacheKeyHash
    {
        size_t operator()(const VKNFramebufferCacheKey& key) const
        {
            return key.GetHash();
        }
    };

	//-------------------
	//VKNFramebufferCache
	//-------------------

    class VKNFramebufferCache final
    {
    public:
        LAMBDA_NO_COPY(VKNFramebufferCache);
        
		VKNFramebufferCache();
		~VKNFramebufferCache();

        void ReleaseAll(VkDevice device);
        void OnReleaseTexture(VkDevice device, const VKNTexture* pTexture);
		void OnReleaseRenderPass(VkDevice device, VkRenderPass renderpass);
        VkFramebuffer GetFramebuffer(const VKNFramebufferCacheKey& key, VkExtent2D extent);
        
    private:
        std::unordered_map<VKNFramebufferCacheKey, VkFramebuffer, VKNFramebufferCacheKeyHash> m_Framebuffers;

	public:
		static VKNFramebufferCache& Get();

	private:
		static VKNFramebufferCache* s_pInstance;
    };
}
