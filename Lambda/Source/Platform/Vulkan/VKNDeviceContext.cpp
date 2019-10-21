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

#define LAMBDA_VK_MAX_COMMANDS 128

namespace Lambda
{
	//----------------
	//VKNDeviceContext
	//----------------

    VKNDeviceContext::VKNDeviceContext(VKNDevice* pDevice, DeviceContextType type)
        : TDeviceContext(pDevice, type),
		m_CommandPool(VK_NULL_HANDLE),
		m_pFrameResources(nullptr),
        m_pCurrentFrameResource(nullptr),
		m_pResourceTracker(nullptr),
        m_RenderPass(VK_NULL_HANDLE),
        m_Framebuffer(VK_NULL_HANDLE),
        m_NumFrameResources(0),
		m_NumCommands(0),
        m_FrameIndex(0),
		m_ContextState(DEVICE_CONTEXT_STATE_WAITING)
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

		//Set type
		m_Type = type;
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


	void VKNDeviceContext::CommitPipelineState()
	{
		if (m_PipelineState)
		{
			//Make sure that we have a commandbuffer
			QueryCommandBuffer();

			//Bind pipeline
			VkPipeline pipeline = m_PipelineState->GetVkPipeline();

			const PipelineStateDesc& desc = m_PipelineState->GetDesc();
			if (desc.Type == PIPELINE_TYPE_GRAPHICS)
				vkCmdBindPipeline(m_pCurrentFrameResource->CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
			else if (desc.Type == PIPELINE_TYPE_COMPUTE)
				vkCmdBindPipeline(m_pCurrentFrameResource->CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
		}
	}


	void VKNDeviceContext::CommitAndTransitionResources()
	{
		//Commit resources
		if (m_ShaderVariableTable)
		{
			m_ShaderVariableTable->CommitAndTransitionResources(this);
			FlushResourceBarriers();

			//Get dynamic offset info
			const uint32* pOffsets	= m_ShaderVariableTable->GetDynamicOffsets();
			uint32 offsetCount		= m_ShaderVariableTable->GetDynamicOffsetCount();

			//Bind descriptorset
			VkDescriptorSet descriptorSet	= m_ShaderVariableTable->GetVkDescriptorSet();
			VkPipelineLayout pipelineLayout = m_PipelineState->GetVkPipelineLayout();

			const PipelineStateDesc& desc = m_PipelineState->GetDesc();
			if (desc.Type == PIPELINE_TYPE_GRAPHICS)
				vkCmdBindDescriptorSets(m_pCurrentFrameResource->CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, offsetCount, pOffsets);
			else if (desc.Type == PIPELINE_TYPE_COMPUTE)
				vkCmdBindDescriptorSets(m_pCurrentFrameResource->CommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, offsetCount, pOffsets);
		}
	}


	void VKNDeviceContext::CommitVertexBuffers()
	{
		//Make sure that we have a commandbuffer
		QueryCommandBuffer();

		VkBuffer	 buffers[LAMBDA_MAX_VERTEXBUFFER_COUNT];
		VkDeviceSize offsets[LAMBDA_MAX_VERTEXBUFFER_COUNT];
		for (uint32 i = 0; i < m_NumVertexBuffers; i++)
		{
			if (m_VertexBuffers[i])
			{
				buffers[i] = m_VertexBuffers[i]->GetVkBuffer();
				offsets[i] = m_VertexBuffers[i]->GetDynamicOffset();
			}
			else
			{
				buffers[i] = VK_NULL_HANDLE;
				offsets[i] = 0;
			}
		}
		vkCmdBindVertexBuffers(m_pCurrentFrameResource->CommandBuffer, 0, m_NumVertexBuffers, buffers, offsets);
	}


	void VKNDeviceContext::CommitIndexBuffer()
	{
		//Make sure that we have a commandbuffer
		QueryCommandBuffer();

		//Bind indexbuffer
		VkBuffer	buffer		= m_IndexBuffer->GetVkBuffer();
		VkIndexType indexType	= (m_IndexBufferFormat == FORMAT_R32_UINT) ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16;
		vkCmdBindIndexBuffer(m_pCurrentFrameResource->CommandBuffer, buffer, m_IndexBuffer->GetDynamicOffset(), indexType);
	}


	void VKNDeviceContext::CommitRenderTargetsAndDepthStencil()
	{
		//Setup renderpass
		VKNRenderPassCacheKey renderPasskey = {};
		renderPasskey.NumRenderTargets = m_NumRenderTargets;

		//Setup framebuffer
		VKNFramebufferCacheKey framebufferKey = {};
		framebufferKey.NumAttachmentViews = m_NumRenderTargets;
		for (uint32 i = 0; i < m_NumRenderTargets; i++)
		{
			//Get rendertarget and transition
			TransitionTexture(m_RenderTargets[i].Get(), RESOURCE_STATE_RENDERTARGET, VK_REMAINING_MIP_LEVELS);

			//Get view
			framebufferKey.AttachmentViews[i] = m_RenderTargets[i]->GetVkImageView();

			//Get format
			const TextureDesc& desc = m_RenderTargets[i]->GetDesc();
			renderPasskey.RenderTargetFormats[i] = desc.Format;
		}

		//Get depthstencil
		if (m_DepthStencil)
		{
			//Set and transition depthstencil
			TransitionTexture(m_DepthStencil.Get(), RESOURCE_STATE_DEPTH_STENCIL, VK_REMAINING_MIP_LEVELS);
			framebufferKey.AttachmentViews[framebufferKey.NumAttachmentViews++] = m_DepthStencil->GetVkImageView();

			//Get format
			const TextureDesc& desc = m_DepthStencil->GetDesc();
			renderPasskey.DepthStencilFormat = desc.Format;
		}
		else
		{
			framebufferKey.AttachmentViews[framebufferKey.NumAttachmentViews++] = VK_NULL_HANDLE;
			renderPasskey.DepthStencilFormat = FORMAT_UNKNOWN;
		}

		//Get renderpass
		VKNRenderPassCache& renderPassCache = VKNRenderPassCache::Get();
		renderPasskey.SampleCount = m_FrameBufferSampleCount;
		m_RenderPass = renderPassCache.GetRenderPass(renderPasskey);

		//Get framebuffer
		VKNFramebufferCache& framebufferCache = VKNFramebufferCache::Get();
		framebufferKey.RenderPass = m_RenderPass;
		m_Framebuffer = framebufferCache.GetFramebuffer(framebufferKey, { m_FrameBufferWidth, m_FrameBufferHeight });
	}


	void VKNDeviceContext::CommitViewports()
	{
		//Make sure that we have a commandbuffer
		QueryCommandBuffer();

		//Set viewports
		VkViewport views[LAMBDA_MAX_VIEWPORT_COUNT] = {};
		for (uint32 i = 0; i < m_NumViewports; i++)
		{
			views[i].width		=  m_Viewports[i].Width;
			views[i].height		= -m_Viewports[i].Height;
			views[i].minDepth	=  m_Viewports[i].MinDepth;
			views[i].maxDepth	=  m_Viewports[i].MaxDepth;
			views[i].x			=  m_Viewports[i].TopX;
			views[i].y			=  m_Viewports[i].TopY + m_Viewports[i].Height;
		}
		vkCmdSetViewport(m_pCurrentFrameResource->CommandBuffer, 0, m_NumViewports, views);
	}


	void VKNDeviceContext::CommitScissorRects()
	{
		//Make sure that we have a commandbuffer
		QueryCommandBuffer();

		//Set scissorrects
		VkRect2D rects[LAMBDA_MAX_SCISSOR_RECT_COUNT] = {};
		for (uint32 i = 0; i < m_NumScissorRects; i++)
		{
			rects[i].offset.x		= int32(m_ScissorRects[i].X);
			rects[i].offset.y		= int32(m_ScissorRects[i].Y);
			rects[i].extent.height	= uint32(m_ScissorRects[i].Height);
			rects[i].extent.width	= uint32(m_ScissorRects[i].Width);
		}
		vkCmdSetScissor(m_pCurrentFrameResource->CommandBuffer, 0, m_NumScissorRects, rects);
	}
    
    
    void VKNDeviceContext::BlitTexture(VKNTexture* pDst, uint32 dstWidth, uint32 dstHeight, uint32 dstMipLevel, VKNTexture* pSrc, uint32 srcWidth, uint32 srcHeight, uint32 srcMipLevel)
    {
		//Check if we should flush the context
		if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE && m_NumCommands > LAMBDA_VK_MAX_COMMANDS)
		{																					
			this->Flush();																	
		}																					

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

		//Count command
		m_NumCommands++;
    }


	void VKNDeviceContext::ClearRenderTarget(ITexture* pRenderTarget, float color[4])
    {
		//Check if we should flush the context
		if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE && m_NumCommands > LAMBDA_VK_MAX_COMMANDS)
		{
			this->Flush();
		}

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

		//Count command
		m_NumCommands++;
    }
    
    
    void VKNDeviceContext::ClearDepthStencil(ITexture* pDepthStencil, float depth, uint8 stencil)
    {
		//Check if we should flush the context
		if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE && m_NumCommands > LAMBDA_VK_MAX_COMMANDS)
		{
			this->Flush();
		}

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

		//Count command
		m_NumCommands++;
    }

    
	void VKNDeviceContext::SetRendertargets(ITexture* const* ppRenderTargets, uint32 numRenderTargets, ITexture* pDepthStencil)
	{
		//Check if we should flush the context
		if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE && m_NumCommands > LAMBDA_VK_MAX_COMMANDS)
		{
			this->Flush();
		}

		//Call base
		TDeviceContext::SetRendertargets(ppRenderTargets, numRenderTargets, pDepthStencil);

		//Count command
		m_NumCommands++;
	}


	void VKNDeviceContext::SetViewports(const Viewport* pViewports, uint32 numViewports)
    {
		//Check if we should flush the context
		if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE && m_NumCommands > LAMBDA_VK_MAX_COMMANDS)
		{
			this->Flush();
		}

		//Call base
		TDeviceContext::SetViewports(pViewports, numViewports);

		//Count command
		m_NumCommands++;
    }
    
    
    void VKNDeviceContext::SetScissorRects(const Rectangle* pScissorRects, uint32 numRects)
    {
		//Check if we should flush the context
		if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE && m_NumCommands > LAMBDA_VK_MAX_COMMANDS)
		{
			this->Flush();
		}

		//Call base
		TDeviceContext::SetScissorRects(pScissorRects, numRects);

		//Count command
		m_NumCommands++;
    }
    
    
    void VKNDeviceContext::SetPipelineState(IPipelineState* pPipelineState)
    {
		//Check if we should flush the context
		if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE && m_NumCommands > LAMBDA_VK_MAX_COMMANDS)
		{
			this->Flush();
		}

		//Call base
		TDeviceContext::SetPipelineState(pPipelineState);

		//Count command
		m_NumCommands++;
    }

	
	void VKNDeviceContext::SetShaderVariableTable(IShaderVariableTable* pVariableTable)
	{
		//Check if we should flush the context
		if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE && m_NumCommands > LAMBDA_VK_MAX_COMMANDS)
		{
			this->Flush();
		}

		//Call base
		TDeviceContext::SetShaderVariableTable(pVariableTable);

		//Count command
		m_NumCommands++;
	}
    

    void VKNDeviceContext::SetVertexBuffers(IBuffer* const* pBuffers, uint32 numBuffers, uint32 slot)
    {
		//Check if we should flush the context
		if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE && m_NumCommands > LAMBDA_VK_MAX_COMMANDS)
		{
			this->Flush();
		}

		//Call base
		TDeviceContext::SetVertexBuffers(pBuffers, numBuffers, slot);

		//Count command
		m_NumCommands++;
    }
    
    
    void VKNDeviceContext::SetIndexBuffer(IBuffer* pBuffer, Format format)
    {
		//Check if we should flush the context
		if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE && m_NumCommands > LAMBDA_VK_MAX_COMMANDS)
		{
			this->Flush();
		}

		//Call base
		TDeviceContext::SetIndexBuffer(pBuffer, format);

		//Count command
		m_NumCommands++;
    }

	
	void VKNDeviceContext::SetConstantBlocks(ShaderStage stage, uint32 offset, uint32 sizeInBytes, void* pData)
	{
		//Check if we should flush the context
		if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE && m_NumCommands > LAMBDA_VK_MAX_COMMANDS)
		{
			this->Flush();
		}

		//Make sure that we have a commandbuffer
		QueryCommandBuffer();

		VkShaderStageFlags shaderStageFlags = ConvertShaderStages(stage);

		VkPipelineLayout pipelineLayout = m_PipelineState->GetVkPipelineLayout();
		vkCmdPushConstants(m_pCurrentFrameResource->CommandBuffer, pipelineLayout, shaderStageFlags, offset, sizeInBytes, pData);

		//Count command
		m_NumCommands++;
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

		//Check if we should flush the context
		if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE && m_NumCommands > LAMBDA_VK_MAX_COMMANDS)
		{
			this->Flush();
		}

		VKNBuffer* pVkResource = reinterpret_cast<VKNBuffer*>(pResource);
		
		//Update dynamic resource with dynamic offset
		const BufferDesc& bufferDesc = pResource->GetDesc();
		if (bufferDesc.Usage == RESOURCE_USAGE_DEFAULT)
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
            VKNUploadAllocation mem = m_pCurrentFrameResource->BufferUpload->Allocate(pData->SizeInBytes, alignment);
			LAMBDA_ASSERT_PRINT(pData->SizeInBytes <= mem.SizeInBytes, "Vulkan: ResourceData::SizeInBytes is bigger than the allocated space\n");
            memcpy(mem.pHostMemory, pData->pData, pData->SizeInBytes);

            //Copy buffer
            VkBufferCopy copyRegion = {};
            copyRegion.srcOffset    = mem.DeviceOffset;
            copyRegion.dstOffset    = 0;
            copyRegion.size         = pData->SizeInBytes;
            VkBuffer dstBuffer = reinterpret_cast<VkBuffer>(pVkResource->GetNativeHandle());
            vkCmdCopyBuffer(m_pCurrentFrameResource->CommandBuffer, mem.Buffer, dstBuffer, 1, &copyRegion);

			//Count command
			m_NumCommands++;
		}
		else if (bufferDesc.Usage == RESOURCE_USAGE_DYNAMIC)
		{
			LOG_DEBUG_ERROR("Vulkan: RESOURCE_USAGE_DYNAMIC can only be updated with MapBuffer\n");
		}
    }
    
    
    void VKNDeviceContext::UpdateTexture(ITexture* pResource, const ResourceData* pData, uint32 mipLevel)
    {
		LAMBDA_ASSERT_PRINT(pData != nullptr, "Vulkan: pData cannot be nullptr\n");
		LAMBDA_ASSERT_PRINT(pData->pData != nullptr && pData->SizeInBytes != 0, "Vulkan: ResourceData::pData or ResourceData::SizeInBytes cannot be null\n");

		//Check if we should flush the context
		if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE && m_NumCommands > LAMBDA_VK_MAX_COMMANDS)
		{
			this->Flush();
		}

        TransitionTexture(pResource, RESOURCE_STATE_COPY_DEST, VK_REMAINING_MIP_LEVELS);
		FlushResourceBarriers();

        VKNTexture* pVkResource = reinterpret_cast<VKNTexture*>(pResource);
    
		//Get device properties
		VkPhysicalDeviceProperties properties = m_pDevice->GetPhysicalDeviceProperties();
		uint64 alignment = std::max(properties.limits.optimalBufferCopyOffsetAlignment, VkDeviceSize(4));

        //Allocate memory in the uploadbuffer
		VKNUploadAllocation mem = m_pCurrentFrameResource->TextureUpload->Allocate(pData->SizeInBytes, alignment);
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

		//Count command
		m_NumCommands++;
    }
    
    
    void VKNDeviceContext::CopyBuffer(IBuffer* pDst, IBuffer* pSrc)
    {
		//Check if we should flush the context
		if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE && m_NumCommands > LAMBDA_VK_MAX_COMMANDS)
		{
			this->Flush();
		}

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

		//Count command
		m_NumCommands++;
    }


	void VKNDeviceContext::MapBuffer(IBuffer* pBuffer, MapFlag mapFlag, void** ppData)
	{
		//Check if we should flush the context
		if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE && m_NumCommands > LAMBDA_VK_MAX_COMMANDS)
		{
			this->Flush();
		}

		const BufferDesc& desc = pBuffer->GetDesc();
		if (desc.Usage == RESOURCE_USAGE_DYNAMIC)
		{
			if (mapFlag == MAP_FLAG_WRITE)
			{
				if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE)
				{

				}
				else
				{
					LOG_DEBUG_ERROR("Vulkan: Only an ImmediateContext can map with MAP_FLAG_WRITE\n");
				}
			}
			else if (mapFlag == MAP_FLAG_WRITE_DISCARD)
			{
				
			}
		}
		else
		{
			LOG_DEBUG_ERROR("Vulkan: Cannot map a buffer without usage RESOURCE_USAGE_DYNAMIC\n");
		}

		//Count command
		m_NumCommands++;
	}


	void VKNDeviceContext::UnmapBuffer(IBuffer* pBuffer)
	{
		//Check if we should flush the context
		if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE && m_NumCommands > LAMBDA_VK_MAX_COMMANDS)
		{
			this->Flush();
		}

		//Count command
		m_NumCommands++;
	}


	void VKNDeviceContext::ResolveTexture(ITexture* pDst, uint32 dstMipLevel, ITexture* pSrc, uint32 srcMipLevel)
	{
		//LOG_DEBUG_INFO("ResolveTexture()\n");

		//Check if we should flush the context
		if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE && m_NumCommands > LAMBDA_VK_MAX_COMMANDS)
		{
			this->Flush();
		}

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

		//Count command
		m_NumCommands++;
	}


	void VKNDeviceContext::Draw(uint32 vertexCount, uint32 startVertex)
	{
		LAMBDA_ASSERT_PRINT(m_PipelineState, "Vulkan: Draw must have a valid PipelineState bound when called\n");

		PrepareForDraw();
		vkCmdDraw(m_pCurrentFrameResource->CommandBuffer, vertexCount, 1, startVertex, 0);

		//Count command
		m_NumCommands++;
	}

	void VKNDeviceContext::DrawIndexed(uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation)
	{
		LAMBDA_ASSERT_PRINT(m_PipelineState, "Vulkan: DrawIndexed must have a valid PipelineState bound when called\n");

		PrepareForDraw();
		vkCmdDrawIndexed(m_pCurrentFrameResource->CommandBuffer, indexCount, 1, startIndexLocation, baseVertexLocation, 0);

		//Count command
		m_NumCommands++;
	}
    
    
    void VKNDeviceContext::DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation)
    {        
        LAMBDA_ASSERT_PRINT(m_PipelineState, "Vulkan: DrawInstanced must have a valid PipelineState bound when called\n");
        
		PrepareForDraw();
        vkCmdDraw(m_pCurrentFrameResource->CommandBuffer, vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);

		//Count command
		m_NumCommands++;
    }
    
    
    void VKNDeviceContext::DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation)
    {
        LAMBDA_ASSERT_PRINT(m_PipelineState, "Vulkan: DrawIndexedInstanced must have a valid PipelineState bound when called\n");

		PrepareForDraw();
        vkCmdDrawIndexed(m_pCurrentFrameResource->CommandBuffer, indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);

		//Count command
		m_NumCommands++;
    }

	
	void VKNDeviceContext::ExecuteDefferedContext(IDeviceContext* pContext)
	{
		//Check if we should flush the context
		if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE && m_NumCommands > LAMBDA_VK_MAX_COMMANDS)
		{
			this->Flush();
		}

		//TODO:

		//Count command
		m_NumCommands++;
	}
    
    
    void VKNDeviceContext::SetName(const char* pName)
    {
		DeviceObjectBase::SetName(pName);
		if (pName)
		{
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
		//LOG_DEBUG_INFO("VKNDeviceContext::Flush()\n");

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

		//Set commandcount
		m_NumCommands = 0;
    }
    

	void VKNDeviceContext::BeginRenderPass()
	{
        LAMBDA_ASSERT_PRINT(!IsInsideRenderPass(), "Vulkan: EndRenderPass must be called before a new call to BeginRenderPass\n");
		LAMBDA_ASSERT_PRINT(m_RenderPass != VK_NULL_HANDLE && m_Framebuffer != VK_NULL_HANDLE, "Vulkan: SetRenderTargets must be called before BeginRenderPass\n");

		//LOG_DEBUG_INFO("BeginRenderPass()\n");

		//Before starting a renderpass we need to transition all texture resources
		FlushResourceBarriers();

        VkRenderPassBeginInfo info = {};
        info.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.pNext				= nullptr;
        info.renderPass			= m_RenderPass;
        info.framebuffer		= m_Framebuffer;
        info.renderArea.offset	= { 0, 0 };
		info.renderArea.extent	= { m_FrameBufferWidth, m_FrameBufferHeight };
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
		//Check if we should flush the context
		if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE && m_NumCommands > LAMBDA_VK_MAX_COMMANDS)
		{
			this->Flush();
		}

		//Commit
		CommitAndTransitionResources();


		CommitPipelineState();
		CommitVertexBuffers();
		CommitIndexBuffer();
		CommitRenderTargetsAndDepthStencil();
		CommitViewports();
		CommitScissorRects();

		//Draw-calls can only issued inside a renderpass
		if (!IsInsideRenderPass())
			BeginRenderPass();
	}

    
    void VKNDeviceContext::ResetQuery(IQuery* pQuery)
    {
        LAMBDA_ASSERT(pQuery != nullptr);
     
		//Check if we should flush the context
		if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE && m_NumCommands > LAMBDA_VK_MAX_COMMANDS)
		{
			this->Flush();
		}

		//Make sure that we have a commandbuffer
		QueryCommandBuffer();

		//Reset the query
        VKNQuery* pVkQuery = reinterpret_cast<VKNQuery*>(pQuery);
        const QueryDesc& desc   = pVkQuery->GetDesc();
        
        VkQueryPool queryPool   = reinterpret_cast<VkQueryPool>(pVkQuery->GetNativeHandle());
        vkCmdResetQueryPool(m_pCurrentFrameResource->CommandBuffer, queryPool, 0, desc.QueryCount);
        pVkQuery->Reset();

		//Count command
		m_NumCommands++;
    }
    
    
    void VKNDeviceContext::WriteTimeStamp(IQuery* pQuery, PipelineStage stage)
    {
        LAMBDA_ASSERT(pQuery != nullptr);
        
		//Check if we should flush the context
		if (m_Type == DEVICE_CONTEXT_TYPE_IMMEDIATE && m_NumCommands > LAMBDA_VK_MAX_COMMANDS)
		{
			this->Flush();
		}

		//Make sure that we have a commandbuffer
		QueryCommandBuffer();

		//Write timestamp
        VKNQuery* pVkQuery		= reinterpret_cast<VKNQuery*>(pQuery);
        VkQueryPool queryPool	= reinterpret_cast<VkQueryPool>(pVkQuery->GetNativeHandle());
        vkCmdWriteTimestamp(m_pCurrentFrameResource->CommandBuffer, ConvertPipelineStage(stage), queryPool, pVkQuery->GetQueryIndex());
        pVkQuery->NextQuery();

		//Count command
		m_NumCommands++;
    }
}
