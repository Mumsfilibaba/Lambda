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

		size_t	GetHash() const;
		bool	ContainsTexture(const VKNTexture* pTexture) const;

		bool	operator==(const VKNFramebufferCacheKey& other) const;

	public:
		mutable size_t	Hash;
        VkRenderPass	RenderPass;
        uint32			NumAttachmentViews;
        VkImageView		AttachmentViews[(LAMBDA_MAX_RENDERTARGET_COUNT + 1) * 2];
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

        void			ReleaseAll(VkDevice device);
        void			ReleaseAllContainingTexture(VkDevice device, const VKNTexture* pTexture);
        VkFramebuffer	GetFramebuffer(const VKNFramebufferCacheKey& key, uint32 width, uint32 height);
        
    private:
        std::unordered_map<VKNFramebufferCacheKey, VkFramebuffer, VKNFramebufferCacheKeyHash> m_Framebuffers;

	public:
		static VKNFramebufferCache& GetInstance();

	private:
		static VKNFramebufferCache* s_pInstance;
    };
}
