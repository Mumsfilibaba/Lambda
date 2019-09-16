#pragma once
#include "Graphics/IRenderPass.h"
#include <vulkan/vulkan.h>
#include "VKNConversions.inl"

namespace Lambda
{
	//-------------
	//VKNRenderPass
	//-------------

	class VKNRenderPass final : public IRenderPass
	{
	public:
		LAMBDA_NO_COPY(VKNRenderPass);

		VKNRenderPass(const RenderPassDesc& desc);
		~VKNRenderPass() = default;

		virtual void			SetRenderTargets(const ITexture* const* const ppRenderTargets, uint32 numRenderTargets, const ITexture* pDepthStencil) override final;
		virtual void			SetClearValues(float color[4], float depth, uint8 stencil) override final;
		virtual void*			GetNativeHandle() const override final;
		virtual RenderPassDesc	GetDesc() const override final;

        void Destroy(VkDevice device);

        VkRenderPass			GetRenderPass() const;
        VkFramebuffer			GetFramebuffer() const;
        VkExtent2D				GetFramebufferExtent() const;
        VkSampleCountFlagBits	GetSampleCount() const;
        const VkClearValue*		GetClearValues() const;
        uint32					GetAttachmentCount() const;

	private:
		void Init(const RenderPassDesc& desc);

	private:
		VkRenderPass	m_RenderPass;
		VkFramebuffer	m_Framebuffer;
		VkExtent2D		m_FramebufferExtent;
        RenderPassDesc	m_Desc;
		VkClearValue	m_ClearValues[LAMBDA_MAX_RENDERTARGET_COUNT + 1];
	};


	inline VkRenderPass VKNRenderPass::GetRenderPass() const
	{
		return m_RenderPass;
	}


	inline VkFramebuffer VKNRenderPass::GetFramebuffer() const
	{
		return m_Framebuffer;
	}


	inline VkExtent2D VKNRenderPass::GetFramebufferExtent() const
	{
		return m_FramebufferExtent;
	}


	inline VkSampleCountFlagBits VKNRenderPass::GetSampleCount() const
	{
		return ConvertSampleCount(m_Desc.SampleCount);
	}


	inline const VkClearValue* VKNRenderPass::GetClearValues() const
	{
		return m_ClearValues;
	}
	
	
	inline uint32 VKNRenderPass::GetAttachmentCount() const
	{
		return m_Desc.NumRenderTargets + ((m_Desc.DepthStencil.Format != FORMAT_UNKNOWN) ? 1 : 0);
	}
}
