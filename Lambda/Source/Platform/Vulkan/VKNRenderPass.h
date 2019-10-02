#pragma once
#include "Graphics/Core/IRenderPass.h"
#include <vulkan/vulkan.h>
#include "VKNDeviceObject.h"
#include "VKNConversions.inl"

namespace Lambda
{
	class VKNDevice;

	//-------------
	//VKNRenderPass
	//-------------

	class VKNRenderPass final : public VKNDeviceObject<IRenderPass>
	{
	public:
		LAMBDA_NO_COPY(VKNRenderPass);

		VKNRenderPass(VKNDevice* pDevice, const RenderPassDesc& desc);
		~VKNRenderPass();

		virtual void SetRenderTargets(const ITexture* const* const ppRenderTargets, uint32 numRenderTargets, const ITexture* pDepthStencil) override final;
		virtual void SetClearValues(float color[4], float depth, uint8 stencil) override final;
		virtual void* GetNativeHandle() const override final;
		virtual const RenderPassDesc& GetDesc() const override final;

		inline VkRenderPass	GetVkRenderPass() const { return m_RenderPass; }
		inline VkFramebuffer GetVkFramebuffer() const { return m_Framebuffer; }
		inline VkExtent2D GetFramebufferExtent() const { return m_FramebufferExtent; }
		inline VkSampleCountFlagBits GetSampleCount() const { return ConvertSampleCount(m_Desc.SampleCount); }
		inline const VkClearValue* GetClearValues() const { return m_ClearValues; }
		inline uint32 GetAttachmentCount() const { return m_Desc.NumRenderTargets + ((m_Desc.DepthStencil.Format != FORMAT_UNKNOWN) ? 1 : 0); }
	private:
		void Init(const RenderPassDesc& desc);
	private:
		VkRenderPass	m_RenderPass;
		VkFramebuffer	m_Framebuffer;
		VkExtent2D		m_FramebufferExtent;
        RenderPassDesc	m_Desc;
		VkClearValue	m_ClearValues[LAMBDA_MAX_RENDERTARGET_COUNT + 1];
	};
}
