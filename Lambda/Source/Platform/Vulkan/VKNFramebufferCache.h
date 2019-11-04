#pragma once
#include <unordered_map>
#include "Vulkan.h"

namespace Lambda
{
	class VKNDevice;
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
		bool ContainsImageView(VkImageView view) const;
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
        
		VKNFramebufferCache(VKNDevice* pVkDevice);
		~VKNFramebufferCache();

        void ReleaseAll();
        void OnReleaseImageView(VkImageView view);
		void OnReleaseRenderPass(VkRenderPass renderpass);
        VkFramebuffer GetFramebuffer(const VKNFramebufferCacheKey& key, VkExtent2D extent);
    private:
		VKNDevice* m_pDevice;
        std::unordered_map<VKNFramebufferCacheKey, VkFramebuffer, VKNFramebufferCacheKeyHash> m_Framebuffers;
	public:
		static VKNFramebufferCache& Get();
	private:
		static VKNFramebufferCache* s_pInstance;
    };
}
