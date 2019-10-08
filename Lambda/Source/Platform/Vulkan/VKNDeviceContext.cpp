#include "LambdaPch.h"
#include "Utilities/MathHelper.h"
#include "VKNDeviceContext.h"
#include "VKNDevice.h"
#include "VKNBuffer.h"
#include "VKNSamplerState.h"
#include "VKNTexture.h"
#include "VKNFramebuffer.h"
#include "VKNQuery.h"
#include "VKNRenderPass.h"
#include "VKNConversions.inl"

namespace Lambda
{
	//----------------
	//VKNDeviceContext
	//----------------

    VKNDeviceContext::VKNDeviceContext(VKNDevice* pDevice, IVKNAllocator* pAllocator, DeviceContextType type)
        : DeviceObjectBase<VKNDevice, IDeviceContext>(pDevice),
		m_CommandPool(VK_NULL_HANDLE),
        m_pCommandBuffers(nullptr),
        m_CurrentCommandBuffer(VK_NULL_HANDLE),
        m_pFences(nullptr),
        m_CurrentFence(VK_NULL_HANDLE),
        m_RenderPass(VK_NULL_HANDLE),
        m_Framebuffer(VK_NULL_HANDLE),
		m_pBufferUpload(nullptr),
		m_pTextureUpload(nullptr),
        m_PipelineState(nullptr),
		m_VariableTable(nullptr),
        m_NumCommandBuffers(0),
        m_FrameIndex(0),
        m_Type(DEVICE_CONTEXT_TYPE_UNKNOWN),
		m_Name()
    {
		//Add a ref to the refcounter
		this->AddRef();
        Init(pAllocator, type);
    }


    VKNDeviceContext::~VKNDeviceContext()
    {
        if (m_pBufferUpload)
        {
            m_pBufferUpload->Destroy(m_pDevice->GetVkDevice());
            m_pBufferUpload = nullptr;
        }
        if (m_pTextureUpload)
        {
            m_pTextureUpload->Destroy(m_pDevice->GetVkDevice());
            m_pTextureUpload = nullptr;
        }
        //Delete commandbuffers
        if (m_pCommandBuffers)
        {
            vkFreeCommandBuffers(m_pDevice->GetVkDevice(), m_CommandPool, m_NumCommandBuffers, m_pCommandBuffers);
        }
        SafeDeleteArr(m_pCommandBuffers);
        //Delete fences
        if (m_pFences)
        {
            for (uint32 i = 0; i < m_NumCommandBuffers; i++)
            {
                vkDestroyFence(m_pDevice->GetVkDevice(), m_pFences[i], nullptr);
                m_pFences[i] = VK_NULL_HANDLE;
            }
        }
        SafeDeleteArr(m_pFences);
        if (m_CommandPool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(m_pDevice->GetVkDevice(), m_CommandPool, nullptr);
            m_CommandPool = VK_NULL_HANDLE;
        }

        LOG_DEBUG_INFO("Vulkan: Destroyed CommandList '%s'\n", m_Name.c_str());
    }
    

    void VKNDeviceContext::Init(IVKNAllocator* pAllocator, DeviceContextType type)
    {
        //Get queuefamiliy indices
        QueueFamilyIndices familyIndices = m_pDevice->GetQueueFamilyIndices();
        
		//Create commandpool
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.pNext = nullptr;
        if (type == DEVICE_CONTEXT_TYPE_GRAPHICS)
            poolInfo.queueFamilyIndex = familyIndices.GraphicsFamily;
        else
            poolInfo.queueFamilyIndex = uint32(-1);
    
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
        const DeviceDesc& deviceDesc = m_pDevice->GetDesc();
        m_NumCommandBuffers = deviceDesc.BackBufferCount;
        m_pCommandBuffers = DBG_NEW VkCommandBuffer[m_NumCommandBuffers];
        
		//Allocate commandbuffer
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool			= m_CommandPool;
        allocInfo.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount	= 1;
        for (uint32 i = 0; i < m_NumCommandBuffers; i++)
        {
            if (vkAllocateCommandBuffers(m_pDevice->GetVkDevice(), &allocInfo, &m_pCommandBuffers[i]) != VK_SUCCESS)
            {
                LOG_DEBUG_ERROR("Vulkan: Failed to create commandbuffer\n");
                return;
            }
            else
            {
                LOG_DEBUG_INFO("Vulkan: Created commandbuffer\n");
            }
        }
        
        
        //Setup fences
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.pNext = nullptr;
        fenceInfo.flags = 0;

        m_pFences = DBG_NEW VkFence[m_NumCommandBuffers];
        for (uint32 i = 0; i < m_NumCommandBuffers; i++)
        {
            //Create fence
            if (vkCreateFence(m_pDevice->GetVkDevice(), &fenceInfo, nullptr, &m_pFences[i]) != VK_SUCCESS)
            {
                LOG_DEBUG_ERROR("Vulkan: Failed to create fence\n");
                return;
            }
            else
            {
                m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_FENCE, (uint64)m_pFences[i], "Fence[" +  std::to_string(i) + "]");
            }
        }

        //Init uploadbuffers
		m_pBufferUpload		= DBG_NEW VKNUploadBuffer(pAllocator, MB(1));
		m_pTextureUpload	= DBG_NEW VKNUploadBuffer(pAllocator, MB(16));
    }


	inline void VKNDeviceContext::CommitAndTransitionResources()
	{
		m_VariableTable->CommitAndTransitionResources();
		const uint32* pOffsets	= m_VariableTable->GetDynamicOffsets();
		uint32 offsetCount		= m_VariableTable->GetDynamicOffsetCount();

		VkDescriptorSet descriptorSet	= m_VariableTable->GetVkDescriptorSet();
		VkPipelineLayout pipelineLayout = m_PipelineState->GetVkPipelineLayout();

		const PipelineStateDesc& desc = m_PipelineState->GetDesc();
		if (desc.Type == PIPELINE_TYPE_GRAPHICS)
			vkCmdBindDescriptorSets(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, offsetCount, pOffsets);
		else if (desc.Type == PIPELINE_TYPE_COMPUTE)
			vkCmdBindDescriptorSets(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, offsetCount, pOffsets);
	}
    
    
    void VKNDeviceContext::BlitTexture(VKNTexture* pDst, uint32 dstWidth, uint32 dstHeight, uint32 dstMipLevel, VKNTexture* pSrc, uint32 srcWidth, uint32 srcHeight, uint32 srcMipLevel)
    {
        LAMBDA_ASSERT_PRINT(m_RenderPass == VK_NULL_HANDLE, "Vulkan: EndRenderPass must be called before BlitTexture\n");
        
        VkImageBlit blitInfo = {};
        blitInfo.srcOffsets[0] = { 0, 0, 0 };
        blitInfo.srcOffsets[1] = { int32(srcWidth), int32(srcHeight), 1 };
        blitInfo.srcSubresource.aspectMask = pSrc->GetAspectFlags();
        blitInfo.srcSubresource.mipLevel = srcMipLevel;
        blitInfo.srcSubresource.baseArrayLayer = 0;
        blitInfo.srcSubresource.layerCount = 1;
        blitInfo.dstOffsets[0] = { 0, 0, 0 };
        blitInfo.dstOffsets[1] = { int32(dstWidth), int32(dstHeight), 1 };
        blitInfo.dstSubresource.aspectMask = pDst->GetAspectFlags();
        blitInfo.dstSubresource.mipLevel = dstMipLevel;
        blitInfo.dstSubresource.baseArrayLayer = 0;
        blitInfo.dstSubresource.layerCount = 1;
    
        VkImage srcImage = reinterpret_cast<VkImage>(pSrc->GetNativeHandle());
        VkImage dstImage = reinterpret_cast<VkImage>(pDst->GetNativeHandle());
        vkCmdBlitImage(m_CurrentCommandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitInfo, VK_FILTER_LINEAR);
    }


	void VKNDeviceContext::ClearRenderTarget(ITexture* pRenderTarget, float color[4])
    {
        LAMBDA_ASSERT_PRINT(m_RenderPass == VK_NULL_HANDLE, "Vulkan: EndRenderPass must be called before ClearRenderTarget\n");
        
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
        vkCmdClearColorImage(m_CurrentCommandBuffer, vkRenderTarget, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &col, 1, &imageSubresourceRange);
    }
    
    
    void VKNDeviceContext::ClearDepthStencil(ITexture* pDepthStencil, float depth, uint8 stencil)
    {
         LAMBDA_ASSERT_PRINT(m_RenderPass == VK_NULL_HANDLE, "Vulkan: EndRenderPass must be called before ClearDepthStencil\n");
        
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
        vkCmdClearDepthStencilImage(m_CurrentCommandBuffer, vkDepthStencil, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &value, 1, &imageSubresourceRange);
    }


    void VKNDeviceContext::SetClearColor(float color[4])
    {
        memcpy(m_ClearColor.float32, color, sizeof(float)*4);
    }
    
    
    void VKNDeviceContext::SetClearDepthStencil(float depth, uint8 stencil)
    {
        m_ClearDepthStencil.depth   = depth;
        m_ClearDepthStencil.stencil = uint32(stencil);
    }

    
    void VKNDeviceContext::SetViewport(const Viewport& viewport)
    {
        VkViewport view = {};
        view.width      = viewport.Width;
        view.height     = -viewport.Height;
        view.minDepth   = viewport.MinDepth;
        view.maxDepth   = viewport.MaxDepth;
        view.x          = viewport.TopX;
		view.y			= viewport.TopY + viewport.Height;
        
        vkCmdSetViewport(m_CurrentCommandBuffer, 0, 1, &view);
    }
    
    
    void VKNDeviceContext::SetScissorRect(const Rectangle& scissorRect)
    {
        VkRect2D rect = {};
        rect.extent.height  = uint32(scissorRect.Height);
        rect.extent.width   = uint32(scissorRect.Width);
        rect.offset.x       = int32(scissorRect.X);
        rect.offset.y       = int32(scissorRect.Y);
        
        vkCmdSetScissor(m_CurrentCommandBuffer, 0, 1, &rect);
    }
    
    
    void VKNDeviceContext::SetPipelineState(IPipelineState* pPipelineState)
    {
		VKNPipelineState* pVkVariableTable = reinterpret_cast<VKNPipelineState*>(pPipelineState);
		pVkVariableTable->AddRef();
		m_PipelineState = pVkVariableTable;
        
		VkPipeline pipeline = m_PipelineState->GetVkPipeline();

		const PipelineStateDesc& desc = m_PipelineState->GetDesc();
		if (desc.Type == PIPELINE_TYPE_GRAPHICS)
			vkCmdBindPipeline(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		else if (desc.Type == PIPELINE_TYPE_COMPUTE)
			vkCmdBindPipeline(m_CurrentCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
    }

	
	void VKNDeviceContext::SetShaderVariableTable(IShaderVariableTable* pVariableTable)
	{
		VKNShaderVariableTable* pVkVariableTable = reinterpret_cast<VKNShaderVariableTable*>(pVariableTable);
		pVkVariableTable->AddRef();
        m_VariableTable = pVkVariableTable;
	}
    

    void VKNDeviceContext::SetVertexBuffer(IBuffer* pBuffer, uint32 slot)
    {
		VKNBuffer*	pVkBuffer = reinterpret_cast<VKNBuffer*>(pBuffer);
		VkBuffer	buffers[] = { pVkBuffer->GetVkBuffer() };

        VkDeviceSize offsets[] = { pVkBuffer->GetDynamicOffset() };
        vkCmdBindVertexBuffers(m_CurrentCommandBuffer, slot, 1, buffers, offsets);
    }
    
    
    void VKNDeviceContext::SetIndexBuffer(IBuffer* pBuffer, Format format)
    {
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

        vkCmdBindIndexBuffer(m_CurrentCommandBuffer, buffer, pVkBuffer->GetDynamicOffset(), indexType);
    }

	
	void VKNDeviceContext::SetConstantBlocks(ShaderStage stage, uint32 offset, uint32 sizeInBytes, void* pData)
	{
		VkShaderStageFlags shaderStageFlags = ConvertShaderStages(stage);

		VkPipelineLayout pipelineLayout = m_PipelineState->GetVkPipelineLayout();
		vkCmdPushConstants(m_CurrentCommandBuffer, pipelineLayout, shaderStageFlags, offset, sizeInBytes, pData);
	}

    
    DeviceContextType VKNDeviceContext::GetType() const
    {
        return m_Type;
    }
    
    
    void* VKNDeviceContext::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_CurrentCommandBuffer);
    }
    
    
    void VKNDeviceContext::TransitionBuffer(const IBuffer* pBuffer, ResourceState state)
    {
		LAMBDA_ASSERT(pBuffer && state);
    }
    
    
    void VKNDeviceContext::TransitionTexture(const ITexture* pTexture, ResourceState state, uint32 startMipLevel, uint32 numMipLevels)
    {
        LAMBDA_ASSERT_PRINT(m_RenderPass == VK_NULL_HANDLE, "Vulkan: EndRenderPass must be called before calling TransitionTexture\n");
        
        const VKNTexture* pVkTexture = reinterpret_cast<const VKNTexture*>(pTexture);
        VkImageLayout newLayout = ConvertResourceStateToImageLayout(state);

        //Setup barrier
        VkImageMemoryBarrier barrier = {};
        barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout                       = pVkTexture->GetVkImageLayout();
        barrier.newLayout                       = newLayout;
        barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.image                           = pVkTexture->GetVkImage();
        barrier.subresourceRange.aspectMask     = pVkTexture->GetAspectFlags();
        barrier.subresourceRange.baseMipLevel   = startMipLevel;
        
        const TextureDesc& textureDesc = pVkTexture->GetDesc();
        if (numMipLevels == LAMBDA_TRANSITION_ALL_MIPS)
        {
            barrier.subresourceRange.levelCount = textureDesc.MipLevels;
        }
        else
        {
            barrier.subresourceRange.levelCount = numMipLevels;
        }
    
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;
        barrier.srcAccessMask                   = 0;
        barrier.dstAccessMask                   = 0;
    
        //Set flags and stage
        VkPipelineStageFlags sourceStage        = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkPipelineStageFlags destinationStage   = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    
        //Set source- mask and stage
        if (barrier.oldLayout == VK_IMAGE_LAYOUT_UNDEFINED)
        {
            barrier.srcAccessMask = 0;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        }
        else if (barrier.oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (barrier.oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (barrier.newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (barrier.oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
        {
            barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        }
        else if (barrier.oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        }
        else if (barrier.oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
        else
        {
            LOG_DEBUG_ERROR("Vulkan: Unsupported src-image layout transition '%d'\n", barrier.oldLayout);
        }
    
        //Set destination- mask and stage
        if (barrier.oldLayout == VK_IMAGE_LAYOUT_UNDEFINED)
        {
            barrier.dstAccessMask = 0;
            destinationStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        }
        else if (barrier.newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (barrier.newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
        {
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (barrier.newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (barrier.newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
        {
            barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            destinationStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        }
        else if (barrier.newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        }
        else if (barrier.newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        {
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
        else
        {
            LOG_DEBUG_ERROR("Vulkan: Unsupported dst-image layout transition '%d'\n", barrier.newLayout);
        }
    
        vkCmdPipelineBarrier(m_CurrentCommandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        pVkTexture->SetVkImageLayout(barrier.newLayout);
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
			LAMBDA_ASSERT_PRINT(m_RenderPass == VK_NULL_HANDLE, "Vulkan: Only buffers with 'usage=RESOURCE_USAGE_DYNAMIC' can be updated during a RenderPass instance\n");
            
            //Get offset before allocating
            uint64 deviceOffset = m_pBufferUpload->GetDeviceOffset();

            //Allocate memory in the uploadbuffer
            void* pMem = m_pBufferUpload->Allocate(pData->SizeInBytes);
            memcpy(pMem, pData->pData, pData->SizeInBytes);

            //Copy buffer
            VkBufferCopy copyRegion = {};
            copyRegion.srcOffset    = deviceOffset;
            copyRegion.dstOffset    = 0;
            copyRegion.size         = pData->SizeInBytes;

            VkBuffer srcBuffer = m_pBufferUpload->GetVkBuffer();
            VkBuffer dstBuffer = reinterpret_cast<VkBuffer>(pVkResource->GetNativeHandle());
            vkCmdCopyBuffer(m_CurrentCommandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
		}
    }
    
    
    void VKNDeviceContext::UpdateTexture(ITexture* pResource, const ResourceData* pData, uint32 mipLevel)
    {
        LAMBDA_ASSERT_PRINT(m_RenderPass == VK_NULL_HANDLE, "Vulkan: EndRenderPass must be called before CopyBuffer\n");
		LAMBDA_ASSERT_PRINT(pData != nullptr, "Vulkan: pData cannot be nullptr\n");
		LAMBDA_ASSERT_PRINT(pData->pData != nullptr && pData->SizeInBytes != 0, "Vulkan: ResourceData::pData or ResourceData::SizeInBytes cannot be null\n");

        TransitionTexture(pResource, RESOURCE_STATE_COPY_DEST, 0, LAMBDA_TRANSITION_ALL_MIPS);
        VKNTexture* pVkResource = reinterpret_cast<VKNTexture*>(pResource);
    
        //Get offset before allocating
        uint64 offset = m_pTextureUpload->GetDeviceOffset();
    
        //Allocate memory in the uploadbuffer
        void* pMappedMemory = m_pTextureUpload->Allocate(pData->SizeInBytes);
        memcpy(pMappedMemory, pData->pData, pData->SizeInBytes);
    
        //Perform copy
        const TextureDesc& textureDesc = pVkResource->GetDesc();
        VkBufferImageCopy region = {};
        region.bufferOffset                     = offset;
        region.bufferRowLength                  = 0;
        region.bufferImageHeight                = 0;
        region.imageSubresource.aspectMask      = pVkResource->GetAspectFlags();
        region.imageSubresource.mipLevel        = mipLevel;
        region.imageSubresource.baseArrayLayer  = 0;
        region.imageSubresource.layerCount      = 1;
        region.imageOffset                      = { 0, 0, 0 };
        region.imageExtent                      = { textureDesc.Width, textureDesc.Height, textureDesc.Depth };
    
        VkBuffer buffer = m_pTextureUpload->GetVkBuffer();
        VkImage image = pVkResource->GetVkImage();
        vkCmdCopyBufferToImage(m_CurrentCommandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    }
    
    
    void VKNDeviceContext::CopyBuffer(IBuffer* pDst, IBuffer* pSrc)
    {
		LAMBDA_ASSERT_PRINT(m_RenderPass == VK_NULL_HANDLE, "Vulkan: EndRenderPass must be called before CopyBuffer\n");
        
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = pDst->GetDesc().SizeInBytes;
    
        VkBuffer srcBuffer = reinterpret_cast<VkBuffer>(pSrc->GetNativeHandle());
        VkBuffer dstBuffer = reinterpret_cast<VkBuffer>(pDst->GetNativeHandle());
        vkCmdCopyBuffer(m_CurrentCommandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    }
    
    
    void VKNDeviceContext::DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation)
    {        
        LAMBDA_ASSERT_PRINT(m_RenderPass != VK_NULL_HANDLE, "Vulkan: BeginRenderPass must be called before DrawInstanced\n");
        LAMBDA_ASSERT_PRINT(m_PipelineState, "Vulkan: DrawInstanced must have a valid PipelineState bound when called\n");
        
        CommitAndTransitionResources();
        vkCmdDraw(m_CurrentCommandBuffer, vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
    }
    
    
    void VKNDeviceContext::DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation)
    {
        LAMBDA_ASSERT_PRINT(m_RenderPass != VK_NULL_HANDLE, "Vulkan: BeginRenderPass must be called before DrawIndexedInstanced\n");
        LAMBDA_ASSERT_PRINT(m_PipelineState, "Vulkan: DrawIndexedInstanced must have a valid PipelineState bound when called\n");

        CommitAndTransitionResources();
        vkCmdDrawIndexed(m_CurrentCommandBuffer, indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
    }
    
    
    void VKNDeviceContext::SetName(const char* pName)
    {
		if (pName != nullptr)
		{
			m_Name = std::string(pName);
            for (uint32 i = 0; i < m_NumCommandBuffers; i++)
            {
                std::string name = m_Name + "[" + std::to_string(i) + "]";
                m_pDevice->SetVulkanObjectName(VK_OBJECT_TYPE_COMMAND_BUFFER, (uint64)m_pCommandBuffers[i], name);
            }
		}
    }
    
    
    void VKNDeviceContext::End()
    {
        if (vkEndCommandBuffer(m_CurrentCommandBuffer) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to End CommandBuffer\n");
        }
    }
    
    
    void VKNDeviceContext::Begin()
    {
        m_FrameIndex = (m_FrameIndex+1) % m_NumCommandBuffers;
        //Get next buffer
        m_CurrentCommandBuffer = m_pCommandBuffers[m_FrameIndex];
        m_CurrentFence = m_pFences[m_FrameIndex];
        //Wait for last frame
        vkWaitForFences(m_pDevice->GetVkDevice(), 1, &m_CurrentFence, VK_TRUE, std::numeric_limits<uint64_t>::max());
        vkResetFences(m_pDevice->GetVkDevice(), 1, &m_CurrentFence);
        
        //Begin commandbuffer
        VkCommandBufferBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.pNext = nullptr;
        info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        info.pInheritanceInfo = nullptr;
        if (vkBeginCommandBuffer(m_CurrentCommandBuffer, &info) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to Begin CommandBuffer\n");
        }
        
		//Reset dependencies, when mapping a UploadBuffer, reset is called aswell
        m_pBufferUpload->Reset();
		m_pTextureUpload->Reset();
    }
    

    void VKNDeviceContext::Flush()
    {
        //Wait for fence and execute
    }


    void VKNDeviceContext::ClearState()
    {
        //Clear state. e.i Reset all values, clear color etc.
    }
    
	void VKNDeviceContext::BeginRenderPass(const ITexture* const* const ppRenderTargets, uint32 numRenderTargets, const ITexture* pDepthStencil)
	{
        LAMBDA_ASSERT_PRINT(m_RenderPass == VK_NULL_HANDLE, "Vulkan: EndRenderPass must be called before a new call to BeginRenderPass\n");
        LAMBDA_ASSERT_PRINT(m_PipelineState, "Vulkan: PipelineState not bound when BeginRenderPass were called\n");
        LAMBDA_ASSERT_PRINT(numRenderTargets > 0 || pDepthStencil != nullptr, "Vulkan: Cannot begin an empty RenderPass. Needs at least one texture.\n");
            
        VKNFramebufferCacheKey key    = {};
        key.RenderPass = m_RenderPass;

        std::vector<VkClearValue> clearValues;
        uint32 sampleCount      = 1;
        VkExtent2D extent       = { 0, 0 };
        
        //Get all textures to render to
        if (numRenderTargets > 0)
        {
            //Set number of attachments and clearvalues
            key.NumAttachmentViews += numRenderTargets;
            clearValues.resize(key.NumAttachmentViews);
            
            //Get views and set clearvalues
            for (uint32 i = 0; i < numRenderTargets; i++)
            {
                key.AttachmentViews[i] = reinterpret_cast<const VKNTexture*>(ppRenderTargets[i])->GetVkImageView();
                clearValues[i].color = m_ClearColor;
            }
        }
        
        //Get depthstencil
        if (pDepthStencil)
        {
            key.AttachmentViews[key.NumAttachmentViews++] = reinterpret_cast<const VKNTexture*>(pDepthStencil)->GetVkImageView();
            
            VkClearValue clearValue = {};
            clearValue.depthStencil = m_ClearDepthStencil;
            clearValues.emplace_back(clearValue);
            
            const TextureDesc& desc = pDepthStencil->GetDesc();
            extent.width    = desc.Width;
            extent.height   = desc.Height;
            sampleCount     = desc.SampleCount;
        }
        else
        {
            const TextureDesc& desc = ppRenderTargets[0]->GetDesc();
            extent.width    = desc.Width;
            extent.height   = desc.Height;
            sampleCount     = desc.SampleCount;
        }

        
        //Get resolve textures
        if (sampleCount > 1)
        {
            for (uint32 i = 0; i < numRenderTargets; i++)
            {
                const VKNTexture* pResolveResource = reinterpret_cast<const VKNTexture*>(ppRenderTargets[i])->GetResolveResource();
                if (pResolveResource)
                {
                    key.AttachmentViews[key.NumAttachmentViews++] = pResolveResource->GetVkImageView();
                }
            }
        }

        VKNFramebufferCache& fbCache = VKNFramebufferCache::Get();
        m_Framebuffer = fbCache.GetFramebuffer(key, extent.width, extent.height);
        
        VkRenderPassBeginInfo info = {};
        info.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.pNext				= nullptr;
        info.renderPass			= m_RenderPass;
        info.framebuffer		= m_Framebuffer;
        info.renderArea.offset	= { 0, 0 };
        info.renderArea.extent	= extent;
        info.clearValueCount	= uint32(clearValues.size());
        info.pClearValues		= clearValues.data();

        //Set renderpass
        vkCmdBeginRenderPass(m_CurrentCommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
	}


	void VKNDeviceContext::EndRenderPass()
	{
        LAMBDA_ASSERT_PRINT(m_RenderPass != VK_NULL_HANDLE, "Vulkan: EndRenderPass must be called after BeginRenderPass\n");
        
        vkCmdEndRenderPass(m_CurrentCommandBuffer);
        m_RenderPass    = VK_NULL_HANDLE;
        m_Framebuffer   = VK_NULL_HANDLE;
	}
    
    
    void VKNDeviceContext::ResetQuery(IQuery* pQuery)
    {
        LAMBDA_ASSERT(pQuery != nullptr);
        
        VKNQuery* pVkQuery = reinterpret_cast<VKNQuery*>(pQuery);
        
        const QueryDesc& desc   = pVkQuery->GetDesc();
        VkQueryPool queryPool   = reinterpret_cast<VkQueryPool>(pVkQuery->GetNativeHandle());
        vkCmdResetQueryPool(m_CurrentCommandBuffer, queryPool, 0, desc.QueryCount);
        
        pVkQuery->Reset();
    }
    
    
    void VKNDeviceContext::WriteTimeStamp(IQuery* pQuery, PipelineStage stage)
    {
        LAMBDA_ASSERT(pQuery != nullptr);
        
        VKNQuery* pVkQuery = reinterpret_cast<VKNQuery*>(pQuery);
        
        VkQueryPool queryPool = reinterpret_cast<VkQueryPool>(pVkQuery->GetNativeHandle());
        vkCmdWriteTimestamp(m_CurrentCommandBuffer, ConvertPipelineStage(stage), queryPool, pVkQuery->GetQueryIndex());
        
        pVkQuery->NextQuery();
    }
}
