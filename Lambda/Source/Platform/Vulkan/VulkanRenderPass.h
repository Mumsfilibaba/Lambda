#pragma once
#include "Graphics/IRenderPass.h"
#include <vulkan/vulkan.h>
#include "VulkanConversions.inl"

namespace Lambda
{
	class VulkanRenderPass final : public IRenderPass
	{
	public:
		LAMBDA_NO_COPY(VulkanRenderPass);

		VulkanRenderPass(VkDevice device, const RenderPassDesc& desc);
		~VulkanRenderPass() = default;

		virtual void SetRenderTargets(const ITexture* const* const ppRenderTargets, uint32 numRenderTargets, const ITexture* pDepthStencil) override final;
		virtual void SetClearValues(float color[4], float depth, uint8 stencil) override final;
		virtual void* GetNativeHandle() const override final;
		
        void Destroy(VkDevice device);

        VkRenderPass GetRenderPass() const;
        VkFramebuffer GetFramebuffer() const;
        VkExtent2D GetFramebufferExtent() const;
        VkSampleCountFlagBits GetSampleCount() const;
        const VkClearValue* GetClearValues() const;
        uint32 GetAttachmentCount() const;

	private:
		void Init(VkDevice device, const RenderPassDesc& desc);

	private:
		VkDevice m_Device;
		VkRenderPass m_RenderPass;
		VkFramebuffer m_Framebuffer;
		VkExtent2D m_FramebufferExtent;
        RenderPassDesc m_Desc;
		VkClearValue m_ClearValues[LAMBDA_MAX_RENDERTARGET_COUNT + 1];
	};


	inline VkRenderPass VulkanRenderPass::GetRenderPass() const
	{
		return m_RenderPass;
	}


	inline VkFramebuffer VulkanRenderPass::GetFramebuffer() const
	{
		return m_Framebuffer;
	}


	inline VkExtent2D VulkanRenderPass::GetFramebufferExtent() const
	{
		return m_FramebufferExtent;
	}


	inline VkSampleCountFlagBits VulkanRenderPass::GetSampleCount() const
	{
		return ConvertSampleCount(m_Desc.SampleCount);
	}


	inline const VkClearValue* VulkanRenderPass::GetClearValues() const
	{
		return m_ClearValues;
	}
	
	
	inline uint32 VulkanRenderPass::GetAttachmentCount() const
	{
		return m_Desc.NumRenderTargets + ((m_Desc.DepthStencil.Format != FORMAT_UNKNOWN) ? 1 : 0);
	}
}
