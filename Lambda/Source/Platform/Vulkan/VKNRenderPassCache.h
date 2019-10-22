#pragma once
#include <unordered_map>
#include <vulkan/vulkan.h>

namespace Lambda
{
	class VKNDevice;
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

		VKNRenderPassCache(VKNDevice* pDevice);
		~VKNRenderPassCache();

		void ReleaseAll();
		VkRenderPass GetRenderPass(const VKNRenderPassCacheKey& key);

	private:
		VKNDevice* m_pDevice;
		std::unordered_map<VKNRenderPassCacheKey, VkRenderPass, VKNRenderPassCacheKeyHash> m_RenderPasses;

	public:
		static VKNRenderPassCache& Get();

	private:
		static VKNRenderPassCache* s_pInstance;
	};
}
