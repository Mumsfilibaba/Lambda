#pragma once
#include "Graphics/IRenderPass.h"
#include <vulkan/vulkan.h>

namespace Lambda
{
	class VulkanRenderPass : public IRenderPass
	{
	public:
		LAMBDA_NO_COPY(VulkanRenderPass);

		VulkanRenderPass(VkDevice device, const RenderPassDesc& desc);
		~VulkanRenderPass() = default;

		virtual void SetRenderTargets(const ITexture2D* const* const ppRenderTargets, const ITexture2D* pDepthStencil) override final;
		virtual void SetClearValues(float color[4], float depth, uint8 stencil) override final;
		virtual void* GetNativeHandle() const override final;

		void Destroy(VkDevice device);

		VkRenderPass GetRenderPass() const;
		VkFramebuffer GetFramebuffer() const;
		VkExtent2D GetFramebufferExtent() const;
		const VkClearValue* GetClearValues() const;
		uint32 GetAttachmentCount() const;

	private:
		void Init(VkDevice device, const RenderPassDesc& desc);

	private:
		VkDevice m_Device;
		VkRenderPass m_RenderPass;
		VkFramebuffer m_Framebuffer;
		VkExtent2D m_FramebufferExtent;
		uint32 m_RenderTargetCount;
		VkFormat m_DepthStencilFormat;
		ITexture2D* m_pCurrentRenderTargets[LAMBDA_RENDERTARGET_COUNT];
		ITexture2D* m_pCurrentDepthStencil;
		VkClearValue m_ClearValues[LAMBDA_RENDERTARGET_COUNT + 1];
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

	inline const VkClearValue* VulkanRenderPass::GetClearValues() const
	{
		return m_ClearValues;
	}
	
	
	inline uint32 VulkanRenderPass::GetAttachmentCount() const
	{
		return m_RenderTargetCount + ((m_DepthStencilFormat != VK_FORMAT_UNDEFINED) ? 1 : 0);
	}
}