#include "LambdaPch.h"
#include "Utilities/MathHelper.h"
#include "VKNDeviceContext.h"
#include "VKNDevice.h"
#include "VKNQuery.h"
#include "VKNBuffer.h"
#include "VKNTexture.h"
#include "VKNSamplerState.h"
#include "VKNRenderPassCache.h"
#include "VKNFramebufferCache.h"
#include "VKNConversions.inl"

namespace Lambda
{
	//----------------
	//VKNDeviceContext
	//----------------

    VKNDeviceContext::VKNDeviceContext(VKNDevice* pDevice, DeviceContextType type)
        : DeviceObjectBase<VKNDevice, IDeviceContext>(pDevice),
		m_CommandPool(VK_NULL_HANDLE),
		m_pFrameResources(nullptr),
        m_pCurrentFrameResource(nullptr),
		m_pResourceTracker(nullptr),
        m_RenderPass(VK_NULL_HANDLE),
        m_Framebuffer(VK_NULL_HANDLE),
        m_PipelineState(nullptr),
		m_VariableTable(nullptr),
        m_NumFrameResources(0),
        m_FrameIndex(0),
		m_ContextState(DEVICE_CONTEXT_STATE_WAITING),
        m_Type(DEVICE_CONTEXT_TYPE_UNKNOWN),
		m_Name()
    {
		//Add a ref to the refcounter
		this->AddRef();
        Init(type);
    }


    VKNDeviceContext::~VKNDeviceContext()
    {
		//Delete frameresources
		for (uint32 i = 0; i < m_NumFrameResources; i++)
		{
			//Delete uploadbuffers
			SafeDelete(m_pFrameResources[i].BufferUpload);
			SafeDelete(m_pFrameResources[i].TextureUpload);

			//Free commandbuffer
			vkFreeCommandBuffers(m_pDevice->GetVkDevice(), m_CommandPool, 1, &m_pFrameResources[i].CommandBuffer);

			//Delete fence
			if (m_pFrameResources[i].Fence != VK_NULL_HANDLE)
				m_pDevice->SafeReleaseVulkanResource<VkFence>(m_pFrameResources[i].Fence);
		}
        SafeDeleteArr(m_pFrameResources);

		//Destroy commandpool
        if (m_CommandPool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(m_pDevice->GetVkDevice(), m_CommandPool, nullptr);
            m_CommandPool = VK_NULL_HANDLE;
        }

		//Destroy layout-tracker
		SafeDelete(m_pResourceTracker);

        LOG_DEBUG_INFO("Vulkan: Destroyed DeviceContext '%s'\n", m_Name.c_str());
    }
    

    void VKNDeviceContext::Init(DeviceContextType type)
    {
        //Get queuefamiliy indices
        QueueFamilyIndices familyIndices = m_pDevice->GetQueueFamilyIndices();
        
		//Create commandpool
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.pNext = nullptr;
		//We use the graphicsqueue only for now
        poolInfo.queueFamilyIndex = familyIndices.GraphicsFamily;
        if (vkCreateCommandPool(m_pDevice->GetVkDevice(), &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create commandpool\n");
            return;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created commandpool\n");
        }
         
        //Setup commandbuffers
        m_NumFrameResources = FRAMES_AHEAD;
		m_pFrameResources	= DBG_NEW FrameResource[m_NumFrameResources];
        
		//Allocate commandbuffer
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool			= m_CommandPool;
        allocInfo.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount	= 1;
        for (uint32 i = 0; i < m_NumFrameResources; i++)
        {
            if (vkAllocateCommandBuffers(m_pDevice->GetVkDevice(), &allocInfo, &m_pFrameResources[i].CommandBuffer) != VK_SUCCESS)
            {
                LOG_DEBUG_ERROR("Vulkan: Failed to create commandbuffer\n");
                return;
            }
        }
        
		LOG_DEBUG_INFO("Vulkan: Created commandbuffers\n");
        
        //Setup fences
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.pNext = nullptr;
        fenceInfo.flags = 0;
        for (uint32 i = 0; i < m_NumFrameResources; i++)
        {
            //Create fence
            if (vkCreateFence(m_pDevice->GetVkDevice(), &fenceInfo, nullptr, &m_pFrameResources[i].Fence) != VK_SUCCESS)
            {
                LOG_DEBUG_ERROR("Vulkan: Failed to create fence\n");
                return;
            }
            else
            {
				//Set fence name
                m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_FENCE, (uint64)m_pFrameResources[i].Fence, "Fence[" +  std::to_string(i) + "]");
				//Initial fence signal
				m_pDevice->ExecuteCommandBuffer(nullptr, 0, m_pFrameResources[i].Fence);
            }
        }
		
		LOG_DEBUG_INFO("Vulkan: Created fences\n");

        //Init uploadbuffers
		for (uint32 i = 0; i < m_NumFrameResources; i++)
		{
			m_pFrameResources[i].BufferUpload  = DBG_NEW VKNUploadBuffer(m_pDevice, MB(1));
			m_pFrameResources[i].TextureUpload = DBG_NEW VKNUploadBuffer(m_pDevice, MB(16));
		}

		//Create imagelayout-tracker
		m_pResourceTracker = DBG_NEW VKNResourceLayoutTracker();
    }


	void VKNDeviceContext::AddWaitSemaphore(VkSemaphore waitSemaphore, VkPipelineStageFlags waitDstStageMask)
	{
		LAMBDA_ASSERT(waitSemaphore != VK_NULL_HANDLE);

		m_WaitSemaphores.emplace_back(waitSemaphore);
		m_WaitDstStageMasks.emplace_back(waitDstStageMask);
	}


	void VKNDeviceContext::AddSignalSemaphore(VkSemaphore signalSemaphore)
	{
		LAMBDA_ASSERT(signalSemaphore != VK_NULL_HANDLE);
		m_SignalSemaphores.emplace_back(signalSemaphore);
	}


	void VKNDeviceContext::CommitAndTransitionResources()
	{
		//Commit resources
		m_VariableTable->CommitAndTransitionResources(this);
		FlushResourceBarriers();

		//Get dynamic offset info
		const uint32* pOffsets	= m_VariableTable->GetDynamicOffsets();
		uint32 offsetCount		= m_VariableTable->GetDynamicOffsetCount();

		//Bind descriptorset
		VkDescriptorSet descriptorSet	= m_VariableTable->GetVkDescriptorSet();
		VkPipelineLayout pipelineLayout = m_PipelineState->GetVkPipelineLayout();

		const PipelineStateDesc& desc = m_PipelineState->GetDesc();
		if (desc.Type == PIPELINE_TYPE_GRAPHICS)
			vkCmdBindDescriptorSets(m_pCurrentFrameResource->CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, offsetCount, pOffsets);
		else if (desc.Type == PIPELINE_TYPE_COMPUTE)
			vkCmdBindDescriptorSets(m_pCurrentFrameResource->CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, offsetCount, pOffsets);
	}
    
    
    void VKNDeviceContext::BlitTexture(VKNTexture* pDst, uint32 dstWidth, uint32 dstHeight, uint32 dstMipLevel, VKNTexture* pSrc, uint32 srcWidth, uint32 srcHeight, uint32 srcMipLevel)
    {
		//Make sure that we transition resources and that we have a commandbuffer
		FlushResourceBarriers();

        LAMBDA_ASSERT_PRINT(m_RenderPass == VK_NULL_HANDLE, "Vulkan: EndRenderPass must be called before BlitTexture\n");
        
        VkImageBlit blitInfo = {};
        blitInfo.srcOffsets[0] = { 0, 0, 0 };
        blitInfo.srcOffsets[1] = { int32(srcWidth), int32(srcHeight), 1 };
        blitInfo.srcSubresource.aspectMask = pSrc->GetVkAspectFlags();
        blitInfo.srcSubresource.mipLevel = srcMipLevel;
        blitInfo.srcSubresource.baseArrayLayer = 0;
        blitInfo.srcSubresource.layerCount = 1;
        blitInfo.dstOffsets[0] = { 0, 0, 0 };
        blitInfo.dstOffsets[1] = { int32(dstWidth), int32(dstHeight), 1 };
        blitInfo.dstSubresource.aspectMask = pDst->GetVkAspectFlags();
        blitInfo.dstSubresource.mipLevel = dstMipLevel;
        blitInfo.dstSubresource.baseArrayLayer = 0;
        blitInfo.dstSubresource.layerCount = 1;
    
        VkImage srcImage = reinterpret_cast<VkImage>(pSrc->GetNativeHandle());
        VkImage dstImage = reinterpret_cast<VkImage>(pDst->GetNativeHandle());
        vkCmdBlitImage(m_pCurrentFrameResource->CommandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitInfo, VK_FILTER_LINEAR);
    }


	void VKNDeviceContext::ClearRenderTarget(ITexture* pRenderTarget, float color[4])
    {
		//Transition texture before clearing
		TransitionTexture(pRenderTarget, RESOURCE_STATE_RENDERTARGET_CLEAR, VK_REMAINING_MIP_LEVELS);
		FlushResourceBarriers();

        VkClearColorValue col = {};
        memcpy(col.float32, color, sizeof(float)*4);
    
        //Specify what part of an image that is going to be cleared
        VkImageSubresourceRange imageSubresourceRange = {};
        imageSubresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        imageSubresourceRange.baseMipLevel   = 0;
        imageSubresourceRange.levelCount     = 1;
        imageSubresourceRange.baseArrayLayer = 0;
        imageSubresourceRange.layerCount     = 1;

        VkImage vkRenderTarget = reinterpret_cast<VkImage>(pRenderTarget->GetNativeHandle());
        vkCmdClearColorImage(m_pCurrentFrameResource->CommandBuffer, vkRenderTarget, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &col, 1, &imageSubresourceRange);
    }
    
    
    void VKNDeviceContext::ClearDepthStencil(ITexture* pDepthStencil, float depth, uint8 stencil)
    {
		//Transition texture before clearing
		TransitionTexture(pDepthStencil, RESORUCE_STATE_DEPTH_STENCIL_CLEAR, VK_REMAINING_MIP_LEVELS);
		FlushResourceBarriers();

        VkClearDepthStencilValue value = {};
        value.depth     = depth;
        value.stencil   = stencil;

        //Specify what part of an image that is going to be cleared
        VkImageSubresourceRange imageSubresourceRange = {};
        imageSubresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        imageSubresourceRange.baseMipLevel   = 0;
        imageSubresourceRange.levelCount     = 1;
        imageSubresourceRange.baseArrayLayer = 0;
        imageSubresourceRange.layerCount     = 1;
    
        VkImage vkDepthStencil = reinterpret_cast<VkImage>(pDepthStencil->GetNativeHandle());
        vkCmdClearDepthStencilImage(m_pCurrentFrameResource->CommandBuffer, vkDepthStencil, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &value, 1, &imageSubresourceRange);
    }

    
	void VKNDeviceContext::SetRendertargets(const ITexture* const* ppRenderTargets, uint32 numRendertargets, const ITexture* pDepthStencil)
	{
		//Setup renderpass
		VKNRenderPassCacheKey renderPasskey = {};
		renderPasskey.NumRenderTargets = numRendertargets;

		//Setup framebuffer
		VKNFramebufferCacheKey framebufferKey = {};
		framebufferKey.NumAttachmentViews = numRendertargets;
		for (uint32 i = 0; i < numRendertargets; i++)
		{
			//Get rendertarget and transition
			m_ppRenderTargets[i] = reinterpret_cast<const VKNTexture*>(ppRenderTargets[i]);
			TransitionTexture(ppRenderTargets[i], RESOURCE_STATE_RENDERTARGET, VK_REMAINING_MIP_LEVELS);

			//Get view
			framebufferKey.AttachmentViews[i] = m_ppRenderTargets[i]->GetVkImageView();

			//Get format
			const TextureDesc& desc = m_ppRenderTargets[i]->GetDesc();
			renderPasskey.RenderTargetFormats[i] = desc.Format;
		}

		//Get depthstencil
		m_DepthStencil = reinterpret_cast<const VKNTexture*>(pDepthStencil);
		uint32 sampleCount	= 0;
		if (pDepthStencil)
		{
			//Set and transition depthstencil
			framebufferKey.AttachmentViews[framebufferKey.NumAttachmentViews++] = m_DepthStencil->GetVkImageView();
			TransitionTexture(pDepthStencil, RESOURCE_STATE_DEPTH_STENCIL, VK_REMAINING_MIP_LEVELS);

			//Get extent
			const TextureDesc& desc = pDepthStencil->GetDesc();
			m_FramebufferExtent.width	= desc.Width;
			m_FramebufferExtent.height	= desc.Height;
			
			//Get samplecount
			sampleCount		= desc.SampleCount;
			renderPasskey.DepthStencilFormat = desc.Format;
		}
		else
		{
			framebufferKey.AttachmentViews[framebufferKey.NumAttachmentViews++] = VK_NULL_HANDLE;

			//Get extent
			const TextureDesc& desc = ppRenderTargets[0]->GetDesc();
			m_FramebufferExtent.width	= desc.Width;
			m_FramebufferExtent.height	= desc.Height;

			//Get samplecount
			sampleCount		= desc.SampleCount;
			renderPasskey.DepthStencilFormat = FORMAT_UNKNOWN;
		}

		//Get renderpass
		VKNRenderPassCache& renderPassCache = VKNRenderPassCache::Get();
		renderPasskey.SampleCount = sampleCount;
		m_RenderPass = renderPassCache.GetRenderPass(renderPasskey);
		
		//Get framebuffer
		VKNFramebufferCache& framebufferCache = VKNFramebufferCache::Get();
		framebufferKey.RenderPass = m_RenderPass;
		m_Framebuffer = framebufferCache.GetFramebuffer(framebufferKey, m_FramebufferExtent);
	}


	void VKNDeviceContext::SetViewports(const Viewport* pViewport, uint32 numViewports)
    {
		LAMBDA_ASSERT(pViewport != nullptr);
		
		//Make sure that we have a commandbuffer
		QueryCommandBuffer();

		//Set viewports
        VkViewport views[LAMBDA_MAX_VIEWPORT_COUNT] = {};
		for (uint32 i = 0; i < numViewports; i++)
		{
			views[i].width    =  pViewport[i].Width;
			views[i].height   = -pViewport[i].Height;
			views[i].minDepth =  pViewport[i].MinDepth;
			views[i].maxDepth =  pViewport[i].MaxDepth;
			views[i].x        =  pViewport[i].TopX;
			views[i].y		  =  pViewport[i].TopY + pViewport[i].Height;
		}
        vkCmdSetViewport(m_pCurrentFrameResource->CommandBuffer, 0, numViewports, views);
    }
    
    
    void VKNDeviceContext::SetScissorRects(const Rectangle* pScissorRect, uint32 numRects)
    {
		LAMBDA_ASSERT(pScissorRect != nullptr);

		//Make sure that we have a commandbuffer
		QueryCommandBuffer();

		//Set scissorrects
        VkRect2D rects[LAMBDA_MAX_SCISSOR_RECT_COUNT] = {};
		for (uint32 i = 0; i < numRects; i++)
		{
			rects[i].offset.x       = int32(pScissorRect[i].X);
			rects[i].offset.y       = int32(pScissorRect[i].Y);
			rects[i].extent.height  = uint32(pScissorRect[i].Height);
			rects[i].extent.width   = uint32(pScissorRect[i].Width);
		}
        
        vkCmdSetScissor(m_pCurrentFrameResource->CommandBuffer, 0, numRects, rects);
    }
    
    
    void VKNDeviceContext::SetPipelineState(IPipelineState* pPipelineState)
    {
		//Make sure that we have a commandbuffer
		QueryCommandBuffer();

		//Get a ref to the bound pipelinestate
		VKNPipelineState* pVkPipelineState = reinterpret_cast<VKNPipelineState*>(pPipelineState);
		pVkPipelineState->AddRef();
		m_PipelineState = pVkPipelineState;
        
		//Bind pipeline
		VkPipeline pipeline = m_PipelineState->GetVkPipeline();

		const PipelineStateDesc& desc = m_PipelineState->GetDesc();
		if (desc.Type == PIPELINE_TYPE_GRAPHICS)
			vkCmdBindPipeline(m_pCurrentFrameResource->CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		else if (desc.Type == PIPELINE_TYPE_COMPUTE)
			vkCmdBindPipeline(m_pCurrentFrameResource->CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
    }

	
	void VKNDeviceContext::SetShaderVariableTable(IShaderVariableTable* pVariableTable)
	{
		VKNShaderVariableTable* pVkVariableTable = reinterpret_cast<VKNShaderVariableTable*>(pVariableTable);
		pVkVariableTable->AddRef();
        m_VariableTable = pVkVariableTable;
	}
    

    void VKNDeviceContext::SetVertexBuffers(IBuffer* const* pBuffers, uint32 numBuffers, uint32 slot)
    {
		//Make sure that we have a commandbuffer
		QueryCommandBuffer();

		VkBuffer	 buffers[LAMBDA_MAX_VERTEXBUFFER_COUNT];
        VkDeviceSize offsets[LAMBDA_MAX_VERTEXBUFFER_COUNT];
		for (uint32 i = 0; i < numBuffers; i++)
		{
			VKNBuffer* pVkBuffer = reinterpret_cast<VKNBuffer*>(pBuffers[i]);
			buffers[i] = pVkBuffer->GetVkBuffer();
			offsets[i] = pVkBuffer->GetDynamicOffset();
		}

        vkCmdBindVertexBuffers(m_pCurrentFrameResource->CommandBuffer, slot, numBuffers, buffers, offsets);
    }
    
    
    void VKNDeviceContext::SetIndexBuffer(IBuffer* pBuffer, Format format)
    {
		//Make sure that we have a commandbuffer
		QueryCommandBuffer();

		//Bind indexbuffer
		VKNBuffer*	pVkBuffer = reinterpret_cast<VKNBuffer*>(pBuffer);
        VkBuffer	buffer = reinterpret_cast<VkBuffer>(pBuffer->GetNativeHandle());

		VkIndexType indexType = VK_INDEX_TYPE_NONE_NV;
		if (format == FORMAT_R32_UINT)
		{
			indexType = VK_INDEX_TYPE_UINT32;
		}
		else if (format == FORMAT_R16_UINT)
		{
			indexType = VK_INDEX_TYPE_UINT16;
		}
		else
		{
			LOG_DEBUG_ERROR("Vulkan: Only supported index formats are VK_INDEX_TYPE_UINT16 or VK_INDEX_TYPE_UINT32\n");
		}

        vkCmdBindIndexBuffer(m_pCurrentFrameResource->CommandBuffer, buffer, pVkBuffer->GetDynamicOffset(), indexType);
    }

	
	void VKNDeviceContext::SetConstantBlocks(ShaderStage stage, uint32 offset, uint32 sizeInBytes, void* pData)
	{
		//Make sure that we have a commandbuffer
		QueryCommandBuffer();

		VkShaderStageFlags shaderStageFlags = ConvertShaderStages(stage);

		VkPipelineLayout pipelineLayout = m_PipelineState->GetVkPipelineLayout();
		vkCmdPushConstants(m_pCurrentFrameResource->CommandBuffer, pipelineLayout, shaderStageFlags, offset, sizeInBytes, pData);
	}

    
    DeviceContextType VKNDeviceContext::GetType() const
    {
        return m_Type;
    }
    
    
    void* VKNDeviceContext::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_pCurrentFrameResource->CommandBuffer);
    }
    
    
    void VKNDeviceContext::TransitionBuffer(const IBuffer* pBuffer, ResourceState state)
    {
		LAMBDA_ASSERT(pBuffer && state);
    }
    
    
    void VKNDeviceContext::TransitionTexture(const ITexture* pTexture, ResourceState state, uint32 mipLevel)
    {
		const VKNTexture* pVkTexture = reinterpret_cast<const VKNTexture*>(pTexture);
		VkImageLayout newLayout = ConvertResourceStateToImageLayout(state);
		m_pResourceTracker->TransitionImage(pVkTexture->GetVkImage(), pVkTexture->GetVkAspectFlags(), mipLevel, newLayout);
    }


	void VKNDeviceContext::FlushResourceBarriers()
	{
		if (m_pResourceTracker->NeedsFlush())
		{
			//Make sure that we have a commandbuffer
			QueryCommandBuffer();

			//Transitions can only happen outside a renderpass
			if (IsInsideRenderPass())
				EndRenderPass();

			m_pResourceTracker->FlushBarriers(m_pCurrentFrameResource->CommandBuffer);
		}
	}
    
    
    void VKNDeviceContext::UpdateBuffer(IBuffer* pResource, const ResourceData* pData)
    {        
        LAMBDA_ASSERT_PRINT(pData != nullptr, "Vulkan: pData cannot be nullptr\n");
        LAMBDA_ASSERT_PRINT(pData->pData != nullptr && pData->SizeInBytes != 0, "Vulkan: ResourceData::pData or ResourceData::SizeInBytes cannot be null\n");

		VKNBuffer* pVkResource = reinterpret_cast<VKNBuffer*>(pResource);
		
		//Update dynamic resource with dynamic offset
		const BufferDesc& bufferDesc = pResource->GetDesc();
		if (bufferDesc.Usage == RESOURCE_USAGE_DYNAMIC)
		{
			pVkResource->DynamicUpdate(pData);
		}
		else
		{
			//Make sure that we have a commandbuffer
			QueryCommandBuffer();

			//End renderpass before updating default buffer
			if (IsInsideRenderPass())
				EndRenderPass();

			//Get device properties
			VkPhysicalDeviceProperties properties = m_pDevice->GetPhysicalDeviceProperties();
			uint64 alignment = VkDeviceSize(4);
			if (bufferDesc.Flags & BUFFER_FLAGS_CONSTANT_BUFFER)
				alignment = std::max(alignment, properties.limits.minUniformBufferOffsetAlignment);

            //Allocate memory in the uploadbuffer
            UploadAllocation mem = m_pCurrentFrameResource->BufferUpload->Allocate(pData->SizeInBytes, alignment);
            memcpy(mem.pHostMemory, pData->pData, pData->SizeInBytes);

            //Copy buffer
            VkBufferCopy copyRegion = {};
            copyRegion.srcOffset    = mem.DeviceOffset;
            copyRegion.dstOffset    = 0;
            copyRegion.size         = pData->SizeInBytes;
            VkBuffer dstBuffer = reinterpret_cast<VkBuffer>(pVkResource->GetNativeHandle());
            vkCmdCopyBuffer(m_pCurrentFrameResource->CommandBuffer, mem.Buffer, dstBuffer, 1, &copyRegion);
		}
    }
    
    
    void VKNDeviceContext::UpdateTexture(ITexture* pResource, const ResourceData* pData, uint32 mipLevel)
    {
		LAMBDA_ASSERT_PRINT(pData != nullptr, "Vulkan: pData cannot be nullptr\n");
		LAMBDA_ASSERT_PRINT(pData->pData != nullptr && pData->SizeInBytes != 0, "Vulkan: ResourceData::pData or ResourceData::SizeInBytes cannot be null\n");

        TransitionTexture(pResource, RESOURCE_STATE_COPY_DEST, VK_REMAINING_MIP_LEVELS);
		FlushResourceBarriers();

        VKNTexture* pVkResource = reinterpret_cast<VKNTexture*>(pResource);
    
		//Get device properties
		VkPhysicalDeviceProperties properties = m_pDevice->GetPhysicalDeviceProperties();
		uint64 alignment = std::max(properties.limits.optimalBufferCopyOffsetAlignment, VkDeviceSize(4));

        //Allocate memory in the uploadbuffer
		UploadAllocation mem = m_pCurrentFrameResource->TextureUpload->Allocate(pData->SizeInBytes, alignment);
        memcpy(mem.pHostMemory, pData->pData, pData->SizeInBytes);
    
        //Perform copy
        const TextureDesc& textureDesc = pVkResource->GetDesc();
        VkBufferImageCopy region = {};
        region.bufferOffset                     = mem.DeviceOffset;
        region.bufferRowLength                  = 0;
        region.bufferImageHeight                = 0;
        region.imageSubresource.aspectMask      = pVkResource->GetVkAspectFlags();
        region.imageSubresource.mipLevel        = mipLevel;
        region.imageSubresource.baseArrayLayer  = 0;
        region.imageSubresource.layerCount      = 1;
        region.imageOffset                      = { 0, 0, 0 };
        region.imageExtent                      = { textureDesc.Width, textureDesc.Height, textureDesc.Depth };
    
        VkImage	 image	= pVkResource->GetVkImage();
        vkCmdCopyBufferToImage(m_pCurrentFrameResource->CommandBuffer, mem.Buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    }
    
    
    void VKNDeviceContext::CopyBuffer(IBuffer* pDst, IBuffer* pSrc)
    {
		//Make sure that we have a commandbuffer
		QueryCommandBuffer();

		//End renderpass when clearing depthstencil
		if (IsInsideRenderPass())
			EndRenderPass();

		//Copy buffer
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = pDst->GetDesc().SizeInBytes;
    
        VkBuffer srcBuffer = reinterpret_cast<VkBuffer>(pSrc->GetNativeHandle());
        VkBuffer dstBuffer = reinterpret_cast<VkBuffer>(pDst->GetNativeHandle());
        vkCmdCopyBuffer(m_pCurrentFrameResource->CommandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    }


	void VKNDeviceContext::MapBuffer(IBuffer* pBuffer, void** ppData)
	{
	}


	void VKNDeviceContext::Unmap(IBuffer* pBuffer)
	{
	}


	void VKNDeviceContext::ResolveTexture(ITexture* pDst, uint32 dstMipLevel, ITexture* pSrc, uint32 srcMipLevel)
	{
		//LOG_DEBUG_INFO("ResolveTexture()\n");

		//Transition texture before clearing
		TransitionTexture(pDst, RESOURCE_STATE_COPY_DEST,	VK_REMAINING_MIP_LEVELS);
		TransitionTexture(pSrc, RESOURCE_STATE_COPY_SRC,	VK_REMAINING_MIP_LEVELS);
		FlushResourceBarriers();

		//Transitions can only happen outside a renderpass
		if (IsInsideRenderPass())
			EndRenderPass();

		VKNTexture* pVkDst = reinterpret_cast<VKNTexture*>(pDst);
		VKNTexture* pVKSrc = reinterpret_cast<VKNTexture*>(pSrc);
		
		VkImageResolve resolve = {};
		resolve.dstOffset = { 0, 0, 0 };
		resolve.srcOffset = { 0, 0, 0 };
		resolve.extent.width  = pVkDst->GetDesc().Width;
		resolve.extent.height = pVkDst->GetDesc().Height;
		resolve.extent.depth  = 1;
		resolve.dstSubresource.aspectMask	  = pVkDst->GetVkAspectFlags();
		resolve.dstSubresource.baseArrayLayer = 0;
		resolve.dstSubresource.layerCount	  = 1;
		resolve.dstSubresource.mipLevel		  = 0;
		resolve.srcSubresource.aspectMask	  = pVKSrc->GetVkAspectFlags();
		resolve.srcSubresource.baseArrayLayer = 0;
		resolve.srcSubresource.layerCount	  = 1;
		resolve.srcSubresource.mipLevel		  = 0;
		vkCmdResolveImage(m_pCurrentFrameResource->CommandBuffer, pVKSrc->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, pVkDst->GetVkImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &resolve);
	}


	void VKNDeviceContext::Draw(uint32 vertexCount, uint32 startVertex)
	{
		LAMBDA_ASSERT_PRINT(m_PipelineState, "Vulkan: Draw must have a valid PipelineState bound when called\n");

		PrepareForDraw();
		vkCmdDraw(m_pCurrentFrameResource->CommandBuffer, vertexCount, 1, startVertex, 0);
	}

	void VKNDeviceContext::DrawIndexed(uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation)
	{
		LAMBDA_ASSERT_PRINT(m_PipelineState, "Vulkan: DrawIndexed must have a valid PipelineState bound when called\n");

		PrepareForDraw();
		vkCmdDrawIndexed(m_pCurrentFrameResource->CommandBuffer, indexCount, 1, startIndexLocation, baseVertexLocation, 0);
	}
    
    
    void VKNDeviceContext::DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation)
    {        
        LAMBDA_ASSERT_PRINT(m_PipelineState, "Vulkan: DrawInstanced must have a valid PipelineState bound when called\n");
        
		PrepareForDraw();
        vkCmdDraw(m_pCurrentFrameResource->CommandBuffer, vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
    }
    
    
    void VKNDeviceContext::DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation)
    {
        LAMBDA_ASSERT_PRINT(m_PipelineState, "Vulkan: DrawIndexedInstanced must have a valid PipelineState bound when called\n");

		PrepareForDraw();
        vkCmdDrawIndexed(m_pCurrentFrameResource->CommandBuffer, indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
    }

	
	void VKNDeviceContext::ExecuteDefferedContext(IDeviceContext* pContext)
	{
		//TODO:
	}
    
    
    void VKNDeviceContext::SetName(const char* pName)
    {
		if (pName != nullptr)
		{
			m_Name = std::string(pName);
            for (uint32 i = 0; i < m_NumFrameResources; i++)
            {
                std::string name = m_Name + "[" + std::to_string(i) + "]";
				VkCommandBuffer buffer = m_pFrameResources[i].CommandBuffer;
                m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_COMMAND_BUFFER, (uint64)buffer, name);
            }
		}
    }
    
    
    void VKNDeviceContext::QueryCommandBuffer()
    {
		if (m_ContextState == DEVICE_CONTEXT_STATE_WAITING)
		{
			//LOG_DEBUG_INFO("QueryCommandBuffer()\n");

			//Get next buffer
			m_pCurrentFrameResource = &m_pFrameResources[m_FrameIndex];
			//Wait for last frame
			vkWaitForFences(m_pDevice->GetVkDevice(), 1, &m_pCurrentFrameResource->Fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
			vkResetFences(m_pDevice->GetVkDevice(), 1, &m_pCurrentFrameResource->Fence);
        
			//Move to next frame
			m_FrameIndex = (m_FrameIndex+1) % m_NumFrameResources;

			//Begin commandbuffer
			VkCommandBufferBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			info.pNext = nullptr;
			info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			info.pInheritanceInfo = nullptr;
			if (vkBeginCommandBuffer(m_pCurrentFrameResource->CommandBuffer, &info) != VK_SUCCESS)
			{
				LOG_DEBUG_ERROR("Vulkan: Failed to Begin CommandBuffer\n");
			}
        
			//Reset dependencies, when mapping a UploadBuffer, reset is called aswell
			m_pCurrentFrameResource->BufferUpload->Reset();
			m_pCurrentFrameResource->TextureUpload->Reset();

			//Set context state
			m_ContextState = DEVICE_CONTEXT_STATE_RECORDING;
		}
    }


	void VKNDeviceContext::EndCommandBuffer()
	{
		//LOG_DEBUG_INFO("EndCommandBuffer()\n");

		//End renderpass before ending commmandsubmition
		if (IsInsideRenderPass())
			EndRenderPass();

		//End commandbuffer
		if (vkEndCommandBuffer(m_pCurrentFrameResource->CommandBuffer) != VK_SUCCESS)
		{
			LOG_DEBUG_ERROR("Vulkan: Failed to End CommandBuffer\n");
		}

		//Set context state
		m_ContextState = DEVICE_CONTEXT_STATE_WAITING;
	}
    

    void VKNDeviceContext::Flush()
    {
		//LOG_DEBUG_INFO("Flush()\n");

		//Before flushing we need to flush the deffered barriers since the application is expecting all the resources to be in correct layout
		FlushResourceBarriers();
		//End the recording
		EndCommandBuffer();

		LAMBDA_ASSERT_PRINT(m_WaitSemaphores.size() == m_WaitDstStageMasks.size(), "Vulkan: Number of WaitSemaphores and WaitDstStageMasks must be the same\n");

		//Wait for fence and execute
		VkCommandBuffer buffers[] = { m_pCurrentFrameResource->CommandBuffer };

		//submit commandbuffers
		VkSubmitInfo submitInfo = {};
		submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext				= nullptr;
		submitInfo.waitSemaphoreCount	= uint32(m_WaitSemaphores.size());
		submitInfo.pWaitSemaphores		= m_WaitSemaphores.data();
		submitInfo.pWaitDstStageMask	= m_WaitDstStageMasks.data();
		submitInfo.commandBufferCount	= 1;
		submitInfo.pCommandBuffers		= buffers;
		submitInfo.signalSemaphoreCount = uint32(m_SignalSemaphores.size());
		submitInfo.pSignalSemaphores	= m_SignalSemaphores.data();
		m_pDevice->ExecuteCommandBuffer(&submitInfo, 1, m_pCurrentFrameResource->Fence);

		//Clear semaphores
		m_WaitSemaphores.clear();
		m_SignalSemaphores.clear();
		m_WaitDstStageMasks.clear();
    }


    void VKNDeviceContext::ClearState()
    {
        //Clear state. e.i Reset all values, clear color etc.
    }
    

	void VKNDeviceContext::BeginRenderPass()
	{
        LAMBDA_ASSERT_PRINT(!IsInsideRenderPass(), "Vulkan: EndRenderPass must be called before a new call to BeginRenderPass\n");
		LAMBDA_ASSERT_PRINT(m_RenderPass != VK_NULL_HANDLE && m_Framebuffer != VK_NULL_HANDLE, "Vulkan: SetRenderTargets must be called before BeginRenderPass\n");

		//LOG_DEBUG_INFO("BeginRenderPass()\n");

        VkRenderPassBeginInfo info = {};
        info.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.pNext				= nullptr;
        info.renderPass			= m_RenderPass;
        info.framebuffer		= m_Framebuffer;
        info.renderArea.offset	= { 0, 0 };
        info.renderArea.extent	= m_FramebufferExtent;
        info.clearValueCount	= 0;
        info.pClearValues		= nullptr;
        vkCmdBeginRenderPass(m_pCurrentFrameResource->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
		
		//Set context state
		m_ContextState = DEVICE_CONTEXT_STATE_RENDERPASS;
	}


	void VKNDeviceContext::EndRenderPass()
	{
        LAMBDA_ASSERT_PRINT(IsInsideRenderPass(), "Vulkan: EndRenderPass must be called after BeginRenderPass\n");
        
		//LOG_DEBUG_INFO("EndRenderPass()\n");

		//End renderpass
        vkCmdEndRenderPass(m_pCurrentFrameResource->CommandBuffer);
		//Set context state
		m_ContextState = DEVICE_CONTEXT_STATE_RECORDING;
	}


	void VKNDeviceContext::PrepareForDraw()
	{
		//Commit and draw
		CommitAndTransitionResources();

		//Draw-calls can only issued inside a renderpass
		if (!IsInsideRenderPass())
			BeginRenderPass();
	}

    
    void VKNDeviceContext::ResetQuery(IQuery* pQuery)
    {
        LAMBDA_ASSERT(pQuery != nullptr);
     
		//Make sure that we have a commandbuffer
		QueryCommandBuffer();

		//Reset the query
        VKNQuery* pVkQuery = reinterpret_cast<VKNQuery*>(pQuery);
        const QueryDesc& desc   = pVkQuery->GetDesc();
        
        VkQueryPool queryPool   = reinterpret_cast<VkQueryPool>(pVkQuery->GetNativeHandle());
        vkCmdResetQueryPool(m_pCurrentFrameResource->CommandBuffer, queryPool, 0, desc.QueryCount);
        pVkQuery->Reset();
    }
    
    
    void VKNDeviceContext::WriteTimeStamp(IQuery* pQuery, PipelineStage stage)
    {
        LAMBDA_ASSERT(pQuery != nullptr);
        
		//Make sure that we have a commandbuffer
		QueryCommandBuffer();

		//Write timestamp
        VKNQuery* pVkQuery		= reinterpret_cast<VKNQuery*>(pQuery);
        VkQueryPool queryPool	= reinterpret_cast<VkQueryPool>(pVkQuery->GetNativeHandle());
        vkCmdWriteTimestamp(m_pCurrentFrameResource->CommandBuffer, ConvertPipelineStage(stage), queryPool, pVkQuery->GetQueryIndex());
        pVkQuery->NextQuery();
    }
}
