#pragma once
#include <vulkan/vulkan.h>
#include <unordered_map>

namespace Lambda
{
	class VKNTexture;

	//---------------------
	//VKNRenderPassCacheKey
	//---------------------

	struct VKNRenderPassCacheKey
	{
	public:
		VKNRenderPassCacheKey();
		~VKNRenderPassCacheKey() = default;

		size_t	GetHash() const;
		bool	operator==(const VKNRenderPassCacheKey& other) const;
	public:
		mutable size_t	Hash;
		uint32 NumRenderTargets;
		uint32 SampleCount;
		Format DepthStencilFormat;
		Format RenderTargetFormats[LAMBDA_MAX_RENDERTARGET_COUNT];
	};

	//-------------------------
	//VKNRenderPassCacheKeyHash
	//-------------------------

	struct VKNRenderPassCacheKeyHash
	{
		size_t operator()(const VKNRenderPassCacheKey& key) const
		{
			return key.GetHash();
		}
	};

	//------------------
	//VKNRenderPassCache
	//------------------

	class VKNRenderPassCache final
	{
	public:
		LAMBDA_NO_COPY(VKNRenderPassCache);

		VKNRenderPassCache();
		~VKNRenderPassCache();

		void ReleaseAll(VkDevice device);
		VkRenderPass GetRenderPass(const VKNRenderPassCacheKey& key);

	private:
		std::unordered_map<VKNRenderPassCacheKey, VkRenderPass, VKNRenderPassCacheKeyHash> m_RenderPasses;

	public:
		static VKNRenderPassCache& Get();

	private:
		static VKNRenderPassCache* s_pInstance;
	};
}
