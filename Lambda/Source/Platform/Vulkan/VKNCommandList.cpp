#include "LambdaPch.h"
#include "Utilities/MathHelper.h"
#include "VKNCommandList.h"
#include "VKNDevice.h"
#include "VKNUploadBuffer.h"
#include "VKNPipelineState.h"
#include "VKNSamplerState.h"
#include "VKNTexture.h"
#include "VKNFramebuffer.h"
#include "VKNBuffer.h"
#include "VKNRenderPass.h"
#include "VKNPipelineResourceState.h"
#include "VKNConversions.inl"

namespace Lambda
{
	//--------------
	//VKNCommandList
	//--------------

    VKNCommandList::VKNCommandList(IVKNAllocator* pAllocator, CommandListType type)
        : m_CommandPool(VK_NULL_HANDLE),
        m_CommandBuffer(VK_NULL_HANDLE),
		m_pBufferUpload(nullptr),
		m_pTextureUpload(nullptr),
        m_pResourceState(nullptr),
        m_pRenderPass(nullptr),
        m_Type(COMMAND_LIST_TYPE_UNKNOWN),
		m_Name()
    {
        Init(pAllocator, type);
    }

    
    void VKNCommandList::Init(IVKNAllocator* pAllocator, CommandListType type)
    {
		VKNDevice& device = VKNDevice::GetInstance();

        //Get queuefamiliy indices
        QueueFamilyIndices familyIndices = device.GetQueueFamilyIndices();
        
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
        

        if (vkCreateCommandPool(device.GetDevice(), &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
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
        
        if (vkAllocateCommandBuffers(device.GetDevice(), &allocInfo, &m_CommandBuffer) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create commandbuffer\n");
            return;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created commandbuffer\n");
        }

        //Init uploadbuffers
		m_pBufferUpload		= DBG_NEW VKNUploadBuffer(pAllocator, MB(1));
		m_pTextureUpload	= DBG_NEW VKNUploadBuffer(pAllocator, MB(16));
    }


	inline void VKNCommandList::CommitResources()
	{
		m_pResourceState->CommitBindings();

		const uint32* pOffsets = m_pResourceState->GetDynamicOffsets();
		uint32 offsetCount = m_pResourceState->GetDynamicOffsetCount();

		VkDescriptorSet descriptorSet = m_pResourceState->GetDescriptorSet();
		VkPipelineLayout pipelineLayout = m_pResourceState->GetPipelineLayout();
		vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, offsetCount, pOffsets);
	}
    
    
    void VKNCommandList::BlitTexture(VKNTexture* pDst, uint32 dstWidth, uint32 dstHeight, uint32 dstMipLevel, VKNTexture* pSrc, uint32 srcWidth, uint32 srcHeight, uint32 srcMipLevel)
    {
		if (!m_pRenderPass)
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
        
			VkImage srcImage = reinterpret_cast<VkImage>(pSrc->GetNativeHandle());
			VkImage dstImage = reinterpret_cast<VkImage>(pDst->GetNativeHandle());
			vkCmdBlitImage(m_CommandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitInfo, VK_FILTER_LINEAR);
		}
		else
		{
			LOG_DEBUG_ERROR("Vulkan: BlitTexture must be called outside a RenderPass instance");
		}
    }
    
    
    void VKNCommandList::Destroy(VkDevice device)
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
    
    
    void VKNCommandList::ClearRenderTarget(ITexture* pRenderTarget, float color[4])
    {
		if (!m_pRenderPass)
		{
			VkClearColorValue col = {};
			memcpy(col.float32, color, sizeof(float) * 4);
        
			//Specify what part of an image that is going to be cleared
			VkImageSubresourceRange imageSubresourceRange = {};
			imageSubresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
			imageSubresourceRange.baseMipLevel   = 0;
			imageSubresourceRange.levelCount     = 1;
			imageSubresourceRange.baseArrayLayer = 0;
			imageSubresourceRange.layerCount     = 1;

			VkImage vkRenderTarget = reinterpret_cast<VkImage>(pRenderTarget->GetNativeHandle());
			vkCmdClearColorImage(m_CommandBuffer, vkRenderTarget, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &col, 1, &imageSubresourceRange);
		}
		else
		{
			LOG_DEBUG_ERROR("Vulkan: ClearRenderTarget must be called outside a RenderPass instance");
		}
    }
    
    
    void VKNCommandList::ClearDepthStencil(ITexture* pDepthStencil, float depth, uint8 stencil)
    {
		if (!m_pRenderPass)
		{
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
			vkCmdClearDepthStencilImage(m_CommandBuffer, vkDepthStencil, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &value, 1, &imageSubresourceRange);
		}
		else
		{
			LOG_DEBUG_ERROR("Vulkan: ClearDepthStencil must be called outside a RenderPass instance");
		}
    }
    
    
    void VKNCommandList::SetViewport(const Viewport& viewport)
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
    
    
    void VKNCommandList::SetScissorRect(const Rectangle& scissorRect)
    {
        VkRect2D rect = {};
        rect.extent.height  = uint32(scissorRect.Height);
        rect.extent.width   = uint32(scissorRect.Width);
        rect.offset.x       = int32(scissorRect.X);
        rect.offset.y       = int32(scissorRect.Y);
        
        vkCmdSetScissor(m_CommandBuffer, 0, 1, &rect);
    }
    
    
    void VKNCommandList::SetGraphicsPipelineState(IGraphicsPipelineState* pPipelineState)
    {
        VkPipeline pipeline = reinterpret_cast<VkPipeline>(pPipelineState->GetNativeHandle());
        vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    }
    
    
    void VKNCommandList::SetVertexBuffer(IBuffer* pBuffer, uint32 slot)
    {
        VkBuffer buffers[] = { reinterpret_cast<VkBuffer>(pBuffer->GetNativeHandle()) };

        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(m_CommandBuffer, slot, 1, buffers, offsets);
    }
    
    
    void VKNCommandList::SetIndexBuffer(IBuffer* pBuffer)
    {
		//Force uint32 for indices
        VkBuffer buffer = reinterpret_cast<VkBuffer>(pBuffer->GetNativeHandle());
        vkCmdBindIndexBuffer(m_CommandBuffer, buffer, 0, VK_INDEX_TYPE_UINT32);
    }


	void VKNCommandList::SetGraphicsPipelineResourceState(IPipelineResourceState* pResourceState)
	{
		VKNPipelineResourceState* pVkResourceState = reinterpret_cast<VKNPipelineResourceState*>(pResourceState);
		m_pResourceState = pVkResourceState;

        CommitResources();
	}

    
    CommandListType VKNCommandList::GetType() const
    {
        return m_Type;
    }
    
    
    void* VKNCommandList::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_CommandBuffer);
    }
    
    
    void VKNCommandList::TransitionBuffer(const IBuffer* pBuffer, ResourceState state)
    {
		LAMBDA_ASSERT(pBuffer && state);
    }
    
    
    void VKNCommandList::TransitionTexture(const ITexture* pTexture, ResourceState state, uint32 startMipLevel, uint32 numMipLevels)
    {
        const VKNTexture* pVkTexture = reinterpret_cast<const VKNTexture*>(pTexture);
        TextureDesc textureDesc = pVkTexture->GetDesc();

        //Setup barrier
        VkImageMemoryBarrier barrier = {};
        barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout                       = pVkTexture->GetResourceState();
        barrier.newLayout                       = ConvertResourceStateToImageLayout(state);
        barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.image                           = reinterpret_cast<VkImage>(pVkTexture->GetNativeHandle());
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
        pVkTexture->SetResourceState(barrier.newLayout);
    }
    
    
    void VKNCommandList::UpdateBuffer(IBuffer* pResource, const ResourceData* pData)
    {        
		LAMBDA_ASSERT(pData != nullptr);

		VKNBuffer* pVkResource = reinterpret_cast<VKNBuffer*>(pResource);
		
		//Update dynamic resource with dynamic offset
		BufferDesc bufferDesc = pResource->GetDesc();
		if (bufferDesc.Usage == RESOURCE_USAGE_DYNAMIC)
		{
			pVkResource->DynamicUpdate(pData);
		}
		else
		{
			if (!m_pRenderPass)
			{
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
				VkBuffer dstBuffer = reinterpret_cast<VkBuffer>(pVkResource->GetNativeHandle());
				vkCmdCopyBuffer(m_CommandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
			}
			else
			{
				LOG_DEBUG_ERROR("Vulkan: Only buffers with 'usage=RESOURCE_USAGE_DYNAMIC' can be updated during a RenderPass instance\n");
			}
		}
    }
    
    
    void VKNCommandList::UpdateTexture(ITexture* pResource, const ResourceData* pData, uint32 mipLevel)
    {
		LAMBDA_ASSERT(pData != nullptr);

		if (!m_pRenderPass)
		{
			TransitionTexture(pResource, RESOURCE_STATE_COPY_DEST, 0, LAMBDA_TRANSITION_ALL_MIPS);
			VKNTexture* pVkResource = reinterpret_cast<VKNTexture*>(pResource);
        
			//Get offset before allocating
			uint64 offset = m_pTextureUpload->GetOffset();
        
			//Allocate memory in the uploadbuffer
			void* pMappedMemory = m_pTextureUpload->Allocate(pData->SizeInBytes);
			memcpy(pMappedMemory, pData->pData, pData->SizeInBytes);
        
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
		else
		{
			LOG_DEBUG_ERROR("Vulkan: UpdateTexture must be called outside a RenderPass instance\n");
		}
    }
    
    
    void VKNCommandList::CopyBuffer(IBuffer* pDst, IBuffer* pSrc)
    {
		if (!m_pRenderPass)
		{
			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = 0;
			copyRegion.size = pDst->GetDesc().SizeInBytes;
        
			VkBuffer srcBuffer = reinterpret_cast<VkBuffer>(pSrc->GetNativeHandle());
			VkBuffer dstBuffer = reinterpret_cast<VkBuffer>(pDst->GetNativeHandle());
			vkCmdCopyBuffer(m_CommandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
		}
		else
		{
			LOG_DEBUG_ERROR("Vulkan: CopyBuffer must be called outside a RenderPass instance");
		}
    }
    
    
    void VKNCommandList::DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation)
    {        
		if (m_pRenderPass)
		{
            if (m_pResourceState)
            {
                CommitResources();
            }
            else
            {
                LOG_DEBUG_ERROR("Vulkan: DrawInstanced must have a valid PipelineResourceState bound when called\n");
            }
            
			vkCmdDraw(m_CommandBuffer, vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
		}
		else
		{
			LOG_DEBUG_ERROR("Vulkan: DrawInstanced must be called inside a RenderPass instance");
		}
    }
    
    
    void VKNCommandList::DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation)
    {
		if (m_pRenderPass)
		{
            if (m_pResourceState)
            {
                CommitResources();
            }
            else
            {
                LOG_DEBUG_ERROR("Vulkan: DrawInstanced must have a valid PipelineResourceState bound when called\n");
            }
            
			vkCmdDrawIndexed(m_CommandBuffer, indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
		}
		else
		{
			LOG_DEBUG_ERROR("Vulkan: DrawIndexedInstanced must be called inside a RenderPass instance");
		}
    }
    
    
    void VKNCommandList::SetName(const char* pName)
    {
		if (pName != nullptr)
		{
			m_Name = std::string(pName);

			VKNDevice& device = VKNDevice::GetInstance();
			device.SetVulkanObjectName(VK_OBJECT_TYPE_COMMAND_BUFFER, (uint64)m_CommandBuffer, m_Name);
		}
    }
    
    
    void VKNCommandList::Close()
    {
        if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to End CommandBuffer\n");
        }
    }
    
    
    void VKNCommandList::Reset()
    {
		VKNDevice& device = VKNDevice::GetInstance();

        //Reset commandbuffer
        if (vkResetCommandPool(device.GetDevice(), m_CommandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT) != VK_SUCCESS)
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
        
		//Reset dependencies, when mapping a UploadBuffer, reset is called aswell
        m_pBufferUpload->Reset();
		m_pTextureUpload->Reset();
    }
    
    
	void VKNCommandList::BeginRenderPass(IRenderPass* pRenderPass)
	{
		if (!m_pRenderPass)
		{
			VKNRenderPass* pVkRenderPass = reinterpret_cast<VKNRenderPass*>(pRenderPass);
			VkRenderPassBeginInfo info = {};
			info.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			info.pNext				= nullptr;
			info.renderPass			= pVkRenderPass->GetRenderPass();
			info.framebuffer		= pVkRenderPass->GetFramebuffer();
			info.renderArea.offset	= { 0, 0 };
			info.renderArea.extent	= pVkRenderPass->GetFramebufferExtent();
			info.clearValueCount	= pVkRenderPass->GetAttachmentCount();
			info.pClearValues		= pVkRenderPass->GetClearValues();

			//Set renderpass
			m_pRenderPass = pVkRenderPass;
			vkCmdBeginRenderPass(m_CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
		}
		else
		{
			LOG_DEBUG_ERROR("Vulkan: BeginRenderPass must be called outside a RenderPass instance");
		}
	}


	void VKNCommandList::EndRenderPass()
	{
		if (m_pRenderPass)
		{
			vkCmdEndRenderPass(m_CommandBuffer);
			m_pRenderPass = nullptr;
		}
		else
		{
			LOG_DEBUG_ERROR("Vulkan: EndRenderPass must be called inside a RenderPass instance");
		}
	}
}
