#include "LambdaPch.h"
#include "VulkanCommandList.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanUploadBuffer.h"
#include "VulkanPipelineState.h"
#include "VulkanSamplerState.h"
#include "VulkanTexture.h"
#include "VulkanFramebuffer.h"
#include "VulkanBuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanResourceState.h"
#include "VulkanConversions.inl"

namespace Lambda
{
    VulkanCommandList::VulkanCommandList(const VulkanGraphicsDevice* pVkDevice, CommandListType type)
        : m_Device(VK_NULL_HANDLE),
        m_CommandPool(VK_NULL_HANDLE),
        m_CommandBuffer(VK_NULL_HANDLE),
		m_pBufferUpload(nullptr),
		m_pTextureUpload(nullptr),
        m_pResourceState(nullptr),
        m_pRenderPass(nullptr),
        m_Type(COMMAND_LIST_TYPE_UNKNOWN),
		m_Name()
    {
		LAMBDA_ASSERT(pVkDevice != VK_NULL_HANDLE);
        Init(pVkDevice, type);
    }

    
    void VulkanCommandList::Init(const VulkanGraphicsDevice* pVkDevice, CommandListType type)
    {
        //Get queuefamiliy indices
        QueueFamilyIndices familyIndices = pVkDevice->GetQueueFamilyIndices();
        
		//Create commandpool
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.pNext = nullptr;
        
        if (type == COMMAND_LIST_TYPE_GRAPHICS)
        {
            poolInfo.queueFamilyIndex = familyIndices.GraphicsFamily;
        }
        else
        {
            poolInfo.queueFamilyIndex = uint32(-1);
        }
        
        VkDevice device = reinterpret_cast<VkDevice>(pVkDevice->GetNativeHandle());
        if (vkCreateCommandPool(device, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create commandpool\n");
            return;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created commandpool\n");
        }
        
		//Allocate commandbuffer
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool			= m_CommandPool;
        allocInfo.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount	= 1;
        
        if (vkAllocateCommandBuffers(device, &allocInfo, &m_CommandBuffer) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create commandbuffer\n");
            return;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created commandbuffer\n");
            m_Device = device;
        }


        //Init upload buffers
		m_pBufferUpload = DBG_NEW VulkanUploadBuffer(pVkDevice, MB(128));
		m_pTextureUpload = DBG_NEW VulkanUploadBuffer(pVkDevice, MB(128));
    }
    
    
    void VulkanCommandList::BlitTexture(VulkanTexture* pDst, uint32 dstWidth, uint32 dstHeight, uint32 dstMipLevel, VulkanTexture* pSrc, uint32 srcWidth, uint32 srcHeight, uint32 srcMipLevel)
    {
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
        
        vkCmdBlitImage(m_CommandBuffer, pSrc->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, pDst->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitInfo, VK_FILTER_LINEAR);
    }
    
    
    void VulkanCommandList::Destroy(VkDevice device)
    {
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);
        
		if (m_pBufferUpload)
		{
			m_pBufferUpload->Destroy(device);
			m_pBufferUpload = nullptr;
		}
		if (m_pTextureUpload)
		{
			m_pTextureUpload->Destroy(device);
			m_pTextureUpload = nullptr;
		}
        if (m_CommandBuffer != VK_NULL_HANDLE)
        {
            vkFreeCommandBuffers(device, m_CommandPool, 1, &m_CommandBuffer);
            m_CommandBuffer = VK_NULL_HANDLE;
        }
        if (m_CommandPool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(device, m_CommandPool, nullptr);
            m_CommandPool = VK_NULL_HANDLE;
        }
        
        LOG_DEBUG_INFO("Vulkan: Destroyed CommandList '%s'\n", m_Name.c_str());

        delete this;
    }
    
    
    void VulkanCommandList::ClearRenderTarget(ITexture* pRenderTarget, float color[4])
    {
        VkClearColorValue col = {};
		memcpy(col.float32, color, sizeof(float) * 4);
        
        VulkanTexture* pVkRenderTarget = reinterpret_cast<VulkanTexture*>(pRenderTarget);
        
		//Specify what part of an image that is going to be cleared
        VkImageSubresourceRange imageSubresourceRange = {};
        imageSubresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        imageSubresourceRange.baseMipLevel   = 0;
        imageSubresourceRange.levelCount     = 1;
        imageSubresourceRange.baseArrayLayer = 0;
        imageSubresourceRange.layerCount     = 1;

        vkCmdClearColorImage(m_CommandBuffer, pVkRenderTarget->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &col, 1, &imageSubresourceRange);
    }
    
    
    void VulkanCommandList::ClearDepthStencil(ITexture* pDepthStencil, float depth, uint8 stencil)
    {
        VkClearDepthStencilValue value = {};
        value.depth     = depth;
        value.stencil   = stencil;

        VulkanTexture* pVkDepthStencil = reinterpret_cast<VulkanTexture*>(pDepthStencil);
    
        //Specify what part of an image that is going to be cleared
        VkImageSubresourceRange imageSubresourceRange = {};
        imageSubresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        imageSubresourceRange.baseMipLevel   = 0;
        imageSubresourceRange.levelCount     = 1;
        imageSubresourceRange.baseArrayLayer = 0;
        imageSubresourceRange.layerCount     = 1;
        
        vkCmdClearDepthStencilImage(m_CommandBuffer, pVkDepthStencil->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &value, 1, &imageSubresourceRange);
    }
    
    
    void VulkanCommandList::SetViewport(const Viewport& viewport)
    {
        VkViewport view = {};
        view.width      = viewport.Width;
        view.height     = viewport.Height;
        view.minDepth   = viewport.MinDepth;
        view.maxDepth   = viewport.MaxDepth;
        view.x          = viewport.TopX;
        view.y          = viewport.TopY;
        
        vkCmdSetViewport(m_CommandBuffer, 0, 1, &view);
    }
    
    
    void VulkanCommandList::SetScissorRect(const Rectangle& scissorRect)
    {
        VkRect2D rect = {};
        rect.extent.height  = uint32(scissorRect.Height);
        rect.extent.width   = uint32(scissorRect.Width);
        rect.offset.x       = int32(scissorRect.X);
        rect.offset.y       = int32(scissorRect.Y);
        
        vkCmdSetScissor(m_CommandBuffer, 0, 1, &rect);
    }
    
    
    void VulkanCommandList::SetGraphicsPipelineState(IGraphicsPipelineState* pPipelineState)
    {
        VkPipeline pipeline = reinterpret_cast<VkPipeline>(pPipelineState->GetNativeHandle());
        vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    }
    
    
    void VulkanCommandList::SetVertexBuffer(IBuffer* pBuffer, uint32 slot)
    {
        VkBuffer buffers[] = { reinterpret_cast<VkBuffer>(pBuffer->GetNativeHandle()) };

        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(m_CommandBuffer, slot, 1, buffers, offsets);
    }
    
    
    void VulkanCommandList::SetIndexBuffer(IBuffer* pBuffer)
    {
		//Force uint32 for indices
        VkBuffer buffer = reinterpret_cast<VkBuffer>(pBuffer->GetNativeHandle());
        vkCmdBindIndexBuffer(m_CommandBuffer, buffer, 0, VK_INDEX_TYPE_UINT32);
    }


	void VulkanCommandList::SetGraphicsPipelineResourceState(IPipelineResourceState* pResourceState)
	{
		VulkanPipelineResourceState* pVkResourceState = reinterpret_cast<VulkanPipelineResourceState*>(pResourceState);
		m_pResourceState = pVkResourceState;
		pVkResourceState->CommitBindings(m_Device);

		VkDescriptorSet descriptorSet = pVkResourceState->GetDescriptorSet();
		vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pVkResourceState->GetPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr);
	}

    
    CommandListType VulkanCommandList::GetType() const
    {
        return m_Type;
    }
    
    
    void* VulkanCommandList::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_CommandBuffer);
    }
    
    
    void VulkanCommandList::TransitionBuffer(const IBuffer* pBuffer, ResourceState state)
    {
		LAMBDA_ASSERT(pBuffer && state);
    }
    
    
    void VulkanCommandList::TransitionTexture(const ITexture* pTexture, ResourceState state, uint32 startMipLevel, uint32 numMipLevels)
    {
        const VulkanTexture* pVkTexture = reinterpret_cast<const VulkanTexture*>(pTexture);
        TextureDesc textureDesc = pVkTexture->GetDesc();
        
        //Setup barrier
        VkImageMemoryBarrier barrier = {};
        barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout                       = pVkTexture->GetCurrentResourceState();
        barrier.newLayout                       = ConvertResourceStateToImageLayout(state);
        barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.image                           = pVkTexture->GetImage();
        barrier.subresourceRange.aspectMask     = pVkTexture->GetAspectFlags();
        barrier.subresourceRange.baseMipLevel   = startMipLevel;
        
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
        
        
        vkCmdPipelineBarrier(m_CommandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        pVkTexture->SetGraphicsPipelineResourceState(barrier.newLayout);
    }
    
    
    void VulkanCommandList::UpdateBuffer(IBuffer* pResource, const ResourceData* pData)
    {        
		LAMBDA_ASSERT(pData != nullptr);

        //Get offset before allocating
        uint64 offset = m_pBufferUpload->GetOffset(); 
        
		//Allocate memory in the uploadbuffer
        void* pMem = m_pBufferUpload->Allocate(pData->SizeInBytes);
        memcpy(pMem, pData->pData, pData->SizeInBytes);
        
        //Copy buffer
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = offset;
        copyRegion.dstOffset = 0;
        copyRegion.size = pData->SizeInBytes;
        
		VkBuffer srcBuffer = m_pBufferUpload->GetBuffer();
		VkBuffer dstBuffer = reinterpret_cast<VkBuffer>(pResource->GetNativeHandle());
        vkCmdCopyBuffer(m_CommandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    }
    
    
    void VulkanCommandList::UpdateTexture(ITexture* pResource, const ResourceData* pData, uint32 mipLevel)
    {
		LAMBDA_ASSERT(pData != nullptr);

        TransitionTexture(pResource, RESOURCE_STATE_COPY_DEST, 0, LAMBDA_TRANSITION_ALL_MIPS);
        VulkanTexture* pVkResource = reinterpret_cast<VulkanTexture*>(pResource);
        
        //Get offset before allocating
        uint64 offset = m_pTextureUpload->GetOffset();
        
        //Allocate memory in the uploadbuffer
        void* pMem = m_pTextureUpload->Allocate(pData->SizeInBytes);
        memcpy(pMem, pData->pData, pData->SizeInBytes);
        
		//Perform copy
        TextureDesc textureDesc = pVkResource->GetDesc();
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
        
        VkBuffer buffer = m_pTextureUpload->GetBuffer();
        VkImage image = reinterpret_cast<VkImage>(pResource->GetNativeHandle());
        vkCmdCopyBufferToImage(m_CommandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    }
    
    
    void VulkanCommandList::CopyBuffer(IBuffer* pDst, IBuffer* pSrc)
    {
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = pDst->GetDesc().SizeInBytes;
        
		VkBuffer srcBuffer = reinterpret_cast<VkBuffer>(pSrc->GetNativeHandle());
		VkBuffer dstBuffer = reinterpret_cast<VkBuffer>(pDst->GetNativeHandle());
        vkCmdCopyBuffer(m_CommandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    }
    
    
    void VulkanCommandList::DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation)
    {        
        vkCmdDraw(m_CommandBuffer, vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
    }
    
    
    void VulkanCommandList::DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation)
    {
        vkCmdDrawIndexed(m_CommandBuffer, indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
    }
    
    
    void VulkanCommandList::SetName(const char* pName)
    {
		if (pName != nullptr)
		{
			m_Name = std::string(pName);

			VulkanGraphicsDevice* pVkDevice = reinterpret_cast<VulkanGraphicsDevice*>(IGraphicsDevice::GetInstance());
			pVkDevice->SetVulkanObjectName(VK_OBJECT_TYPE_COMMAND_BUFFER, (uint64)m_CommandBuffer, m_Name);
		}
    }
    
    
    void VulkanCommandList::Close()
    {
        if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to End CommandBuffer\n");
        }

		//Do not keep map during present calls
		m_pBufferUpload->Unmap(m_Device);
		m_pTextureUpload->Unmap(m_Device);
    }
    
    
    void VulkanCommandList::Reset()
    {
        //Reset commandbuffer
        if (vkResetCommandPool(m_Device, m_CommandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to Reset CommandBuffer\n");
        }
        
        //Begin commandbuffer
        VkCommandBufferBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.pNext = nullptr;
        info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        info.pInheritanceInfo = nullptr;
        
        if (vkBeginCommandBuffer(m_CommandBuffer, &info) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to Begin CommandBuffer\n");
        }
        
		//Reset dependencies
        m_pBufferUpload->Map(m_Device);
		m_pTextureUpload->Map(m_Device);
    }
    
    
	void VulkanCommandList::BeginRenderPass(IRenderPass* pRenderPass)
	{
		VulkanRenderPass* pVkRenderPass = reinterpret_cast<VulkanRenderPass*>(pRenderPass);
		VkRenderPassBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		info.pNext = nullptr;
		info.renderPass = pVkRenderPass->GetRenderPass();
		info.framebuffer = pVkRenderPass->GetFramebuffer();
		info.renderArea.offset = { 0, 0 };
		info.renderArea.extent = pVkRenderPass->GetFramebufferExtent();
		info.clearValueCount = pVkRenderPass->GetAttachmentCount();
		info.pClearValues = pVkRenderPass->GetClearValues();

		vkCmdBeginRenderPass(m_CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
	}


	void VulkanCommandList::EndRenderPass()
	{
		vkCmdEndRenderPass(m_CommandBuffer);
	}
}
