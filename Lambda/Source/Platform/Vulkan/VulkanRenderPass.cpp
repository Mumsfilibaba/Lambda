#include "LambdaPch.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanTexture.h"
#include "VulkanConversions.inl"

namespace Lambda
{
	VulkanRenderPass::VulkanRenderPass(VkDevice device, const RenderPassDesc& desc)
		: m_Device(VK_NULL_HANDLE),
		m_RenderPass(VK_NULL_HANDLE),
		m_Framebuffer(VK_NULL_HANDLE),
		m_FramebufferExtent(),
		m_RenderTargetCount(0),
		m_DepthStencilFormat(VK_FORMAT_UNDEFINED),
		m_ClearValues()
	{
		assert(device != VK_NULL_HANDLE);
		Init(device, desc);
	}
	
	
	void VulkanRenderPass::SetRenderTargets(const ITexture* const* const ppRenderTargets, const ITexture* pDepthStencil)
	{
		assert(m_Device != VK_NULL_HANDLE);

		VulkanFramebufferCacheKey key = {};
		if (pDepthStencil)
		{
			key.DepthStencilView = reinterpret_cast<const VulkanTexture*>(pDepthStencil)->GetImageView();
			m_FramebufferExtent.width = pDepthStencil->GetWidth();
			m_FramebufferExtent.height = pDepthStencil->GetHeight();
		}
		if (ppRenderTargets)
		{
			for (uint32 i = 0; i < m_RenderTargetCount; i++)
				key.ColorAttachmentViews[i] = reinterpret_cast<const VulkanTexture*>(ppRenderTargets[i])->GetImageView();

			if (!pDepthStencil)
			{
				m_FramebufferExtent.width = ppRenderTargets[0]->GetWidth();
				m_FramebufferExtent.height = ppRenderTargets[0]->GetHeight();
			}
		}

		key.NumColorAttachments = m_RenderTargetCount;
		key.RenderPass = m_RenderPass;

		m_Framebuffer = VulkanFramebufferCache::GetFramebuffer(m_Device, key, m_FramebufferExtent.width, m_FramebufferExtent.height);
	}


	void VulkanRenderPass::SetClearValues(float color[4], float depth, uint8 stencil)
	{
		for (uint32 i = 0; i < m_RenderTargetCount; i++)
			memcpy(m_ClearValues[i].color.float32, color, sizeof(float) * 4);

		m_ClearValues[m_RenderTargetCount].depthStencil.depth = depth;
		m_ClearValues[m_RenderTargetCount].depthStencil.stencil = uint32(stencil);
	}


	void* VulkanRenderPass::GetNativeHandle() const
	{
		return reinterpret_cast<void*>(m_RenderPass);
	}
	
	
	void VulkanRenderPass::Destroy(VkDevice device)
	{
		assert(device != VK_NULL_HANDLE);

		if (m_RenderPass != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(device, m_RenderPass, nullptr);
			m_RenderPass = VK_NULL_HANDLE;
		}

		delete this;
	}


	void VulkanRenderPass::Init(VkDevice device, const RenderPassDesc& desc)
	{
		//Setup color attachments
		std::vector<VkAttachmentReference> colorAttachentRefs;
		std::vector<VkAttachmentDescription> attachments;

		//Number of samples (MSAA)
		VkSampleCountFlagBits sampleCount = ConvertSampleCount(desc.SampleCount);

		//Setup colorattachments
		for (uint32 i = 0; i < desc.NumRenderTargets; i++)
		{
			//Setup attachments
			VkAttachmentDescription colorAttachment = {};
			colorAttachment.flags = 0;
			colorAttachment.format = ConvertResourceFormat(desc.RenderTargets[i].Format);
			colorAttachment.samples = sampleCount;
			colorAttachment.loadOp = ConvertLoadOp(desc.RenderTargets[i].LoadOperation);
			colorAttachment.storeOp = ConvertStoreOp(desc.RenderTargets[i].StoreOperation);
			colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			attachments.push_back(colorAttachment);

			VkAttachmentReference colorAttachmentRef = {};
			colorAttachmentRef.attachment = i;
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			colorAttachentRefs.push_back(colorAttachmentRef);
		}

		//Describe subpass
		VkSubpassDescription subpass = {};
		subpass.flags = 0;
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = uint32(colorAttachentRefs.size());
		subpass.pColorAttachments = colorAttachentRefs.data();
		subpass.preserveAttachmentCount = 0;
		subpass.pPreserveAttachments = nullptr;
		subpass.inputAttachmentCount = 0;
		subpass.pInputAttachments = nullptr;
		subpass.pResolveAttachments = nullptr;

		//Setup depthstencil
		VkAttachmentReference depthAttachmentRef = {};
		if (desc.DepthStencil.Format == FORMAT_UNKNOWN)
		{
			subpass.pDepthStencilAttachment = nullptr;
		}
		else
		{
			VkAttachmentDescription depthAttachment = {};
			depthAttachment.flags = 0;
			depthAttachment.format = m_DepthStencilFormat = ConvertResourceFormat(desc.DepthStencil.Format);
			depthAttachment.samples = sampleCount;
			depthAttachment.loadOp = ConvertLoadOp(desc.DepthStencil.LoadOperation);
			depthAttachment.storeOp = ConvertStoreOp(desc.DepthStencil.StoreOperation);;
			depthAttachment.stencilLoadOp = depthAttachment.loadOp;
			depthAttachment.stencilStoreOp = depthAttachment.storeOp;
			depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			attachments.push_back(depthAttachment);

			depthAttachmentRef.attachment = uint32(attachments.size() - 1);
			depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			subpass.pDepthStencilAttachment = &depthAttachmentRef;
		}


		//Create renderpass
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.flags = 0;
		renderPassInfo.pNext = nullptr;
		renderPassInfo.attachmentCount = uint32(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.pDependencies = nullptr;

		if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create renderpass\n");
		}
		else
		{
			LOG_DEBUG_INFO("Vulkan: Created renderpass\n");

			m_RenderTargetCount = desc.NumRenderTargets;
			m_Device = device;
		}
	}
}