#include "LambdaPch.h"
#include "VKNRenderPass.h"
#include "VKNFramebuffer.h"
#include "VKNTexture.h"
#include "VKNDevice.h"
#include "VKNConversions.inl"

namespace Lambda
{
	//-------------
	//VKNRenderPass
	//-------------

	VKNRenderPass::VKNRenderPass(VKNDevice* pDevice, const RenderPassDesc& desc)
		: DeviceObjectBase<VKNDevice, IRenderPass>(pDevice),
		m_RenderPass(VK_NULL_HANDLE),
		m_Framebuffer(VK_NULL_HANDLE),
		m_FramebufferExtent(),
		m_Desc(),
		m_ClearValues()
	{
		//Add a ref to the refcounter
		this->AddRef();
		Init(desc);
	}
	
	
	VKNRenderPass::~VKNRenderPass()
	{
		if (m_RenderPass != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(m_pDevice->GetVkDevice(), m_RenderPass, nullptr);
			m_RenderPass = VK_NULL_HANDLE;
		}

		LOG_DEBUG_INFO("Vulkan: Destroyed RenderPass\n");
	}



	void VKNRenderPass::Init(const RenderPassDesc& desc)
	{
		//Setup color attachments
		std::vector<VkAttachmentReference>		colorAttachentRefs;
		std::vector<VkAttachmentDescription>	attachments;
		std::vector<VkAttachmentDescription>	resolveAttachments;

		//Number of samples (MSAA)
		VkSampleCountFlagBits sampleCount = ConvertSampleCount(desc.SampleCount);
		VkSampleCountFlagBits highestSampleCount = m_pDevice->GetHighestSampleCount();
		if (sampleCount > highestSampleCount)
		{
			LOG_DEBUG_ERROR("Vulkan: RenderPassDesc::SampleCount (=%u) is higher than the maximum of the device (=%u)\n", sampleCount, highestSampleCount);
			return;
		}


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
			colorAttachment.finalLayout = ConvertResourceStateToImageLayout(desc.RenderTargets[i].FinalState);

			if (desc.RenderTargets[i].Flags & RENDER_PASS_ATTACHMENT_FLAG_RESOLVE && sampleCount > VK_SAMPLE_COUNT_1_BIT)
			{
				VkAttachmentDescription resolveAttachment = colorAttachment;
				resolveAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				resolveAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				resolveAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
				if (desc.RenderTargets[i].FinalState == RESOURCE_STATE_RENDERTARGET_PRESENT)
				{
					colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					resolveAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					resolveAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
				}
				resolveAttachments.emplace_back(resolveAttachment);
			}
			attachments.emplace_back(colorAttachment);

			VkAttachmentReference colorAttachmentRef = {};
			colorAttachmentRef.attachment = i;
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			colorAttachentRefs.emplace_back(colorAttachmentRef);
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
			depthAttachment.format = ConvertResourceFormat(desc.DepthStencil.Format);
			depthAttachment.samples = sampleCount;
			depthAttachment.loadOp = ConvertLoadOp(desc.DepthStencil.LoadOperation);
			depthAttachment.storeOp = ConvertStoreOp(desc.DepthStencil.StoreOperation);;
			depthAttachment.stencilLoadOp = depthAttachment.loadOp;
			depthAttachment.stencilStoreOp = depthAttachment.storeOp;
			depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			if (desc.DepthStencil.Flags & RENDER_PASS_ATTACHMENT_FLAG_RESOLVE)
			{
				VkAttachmentDescription resolveAttachment = depthAttachment;
				resolveAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				resolveAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
				resolveAttachments.emplace_back(resolveAttachment);
			}
			attachments.emplace_back(depthAttachment);

			depthAttachmentRef.attachment = uint32(attachments.size() - 1);
			depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			subpass.pDepthStencilAttachment = &depthAttachmentRef;
		}


		//Setup ResolveTargets
		std::vector<VkAttachmentReference> resolveRefs;
		for (auto& attachment : resolveAttachments)
		{
			attachments.emplace_back(attachment);

			VkAttachmentReference resolveAttachmentRef = {};
			resolveAttachmentRef.attachment = uint32(attachments.size() - 1);
			resolveAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			resolveRefs.push_back(resolveAttachmentRef);
		}
		subpass.pResolveAttachments = resolveRefs.size() > 0 ? resolveRefs.data() : nullptr;


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
		if (vkCreateRenderPass(m_pDevice->GetVkDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to create renderpass\n");
		}
		else
		{
			LOG_DEBUG_INFO("Vulkan: Created renderpass\n");
			m_Desc = desc;
			m_Desc.SampleCount = sampleCount;
		}
	}


	void VKNRenderPass::SetRenderTargets(const ITexture* const* const ppRenderTargets, uint32 numRenderTargets, const ITexture* pDepthStencil)
	{
		LAMBDA_ASSERT(numRenderTargets == m_Desc.NumRenderTargets);

		VKNFramebufferCacheKey key	= {};
		key.RenderPass = m_RenderPass;

        //Get all textures to render to
        uint32 sampleCount = 1;
		if (numRenderTargets > 0)
		{
			for (uint32 i = 0; i < numRenderTargets; i++)
			{
				key.AttachmentViews[i] = reinterpret_cast<const VKNTexture*>(ppRenderTargets[i])->GetVkImageView();
				key.NumAttachmentViews++;
			}

			if (!pDepthStencil)
			{
                const TextureDesc& rtDesc = ppRenderTargets[0]->GetDesc();
				m_FramebufferExtent.width	= rtDesc.Width;
				m_FramebufferExtent.height	= rtDesc.Height;
                
                sampleCount = rtDesc.SampleCount;
			}
		}
		if (pDepthStencil)
		{
			key.AttachmentViews[key.NumAttachmentViews++] = reinterpret_cast<const VKNTexture*>(pDepthStencil)->GetVkImageView();

            const TextureDesc& dsDesc = pDepthStencil->GetDesc();
			m_FramebufferExtent.width	= dsDesc.Width;
			m_FramebufferExtent.height	= dsDesc.Height;
            
            sampleCount = dsDesc.SampleCount;
		}
        
        //Get resolve textures
		if (sampleCount > 1)
		{
			for (uint32 i = 0; i < numRenderTargets; i++)
			{
				if (m_Desc.RenderTargets[i].Flags & RENDER_PASS_ATTACHMENT_FLAG_RESOLVE)
                {
                    const VKNTexture* pResolveResource = reinterpret_cast<const VKNTexture*>(ppRenderTargets[i])->GetResolveResource();
                    if (pResolveResource)
                    {
                        key.AttachmentViews[key.NumAttachmentViews++] = pResolveResource->GetVkImageView();
                    }
                }
			}
		}

		VKNFramebufferCache& fbCache = VKNFramebufferCache::Get();
		m_Framebuffer = fbCache.GetFramebuffer(key, m_FramebufferExtent.width, m_FramebufferExtent.height);
	}


	void VKNRenderPass::SetClearValues(float color[4], float depth, uint8 stencil)
	{
		for (uint32 i = 0; i < m_Desc.NumRenderTargets; i++)
			memcpy(m_ClearValues[i].color.float32, color, sizeof(float) * 4);

		m_ClearValues[m_Desc.NumRenderTargets].depthStencil.depth	= depth;
		m_ClearValues[m_Desc.NumRenderTargets].depthStencil.stencil = uint32(stencil);
	}


	void* VKNRenderPass::GetNativeHandle() const
	{
		return reinterpret_cast<void*>(m_RenderPass);
	}


	const RenderPassDesc& VKNRenderPass::GetDesc() const
	{
		return m_Desc;
	}
}
