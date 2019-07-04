#include "LambdaPch.h"
#include "VulkanCommandList.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanPipelineState.h"
#include "VulkanTexture2D.h"
#include "VulkanFramebufferCache.h"
#include "VulkanBuffer.h"

namespace Lambda
{
    VulkanCommandList::VulkanCommandList(VkDevice device, CommandListType type)
        : m_Device(VK_NULL_HANDLE),
        m_CommandPool(VK_NULL_HANDLE),
        m_CommandBuffer(VK_NULL_HANDLE),
        m_DescriptorSets(),
        m_DescriptorPool(VK_NULL_HANDLE),
        m_Type(COMMAND_LIST_TYPE_UNKNOWN)
    {
        Init(device, type);
    }

    
    void VulkanCommandList::Init(VkDevice device, CommandListType type)
    {
        assert(IGraphicsDevice::GetInstance() != nullptr);
        
        //Get queuefamiliy indices
        QueueFamilyIndices familyIndices = reinterpret_cast<VulkanGraphicsDevice*>(IGraphicsDevice::GetInstance())->GetQueueFamilyIndices();
        
        //Setup commandpool
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.pNext = nullptr;
        
        if (type == COMMAND_LIST_TYPE_GRAPHICS)
            poolInfo.queueFamilyIndex = familyIndices.GraphicsFamily;
        else
            poolInfo.queueFamilyIndex = -1;
        
        //Create commandpool
        if (vkCreateCommandPool(device, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create commandpool\n");
            return;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created commandpool\n");
        }
        
        //Setup commandbuffer
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;
        
        //Allocate commandbuffer
        if (vkAllocateCommandBuffers(device, &allocInfo, &m_CommandBuffer) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create commandbuffer\n");
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created commandbuffer\n");
            
            //Store device
            m_Device = device;
            
            //Init upload buffers
            VkPhysicalDevice adapter = reinterpret_cast<VulkanGraphicsDevice*>(IGraphicsDevice::GetInstance())->GetAdapter();
            if (!m_BufferUpload.Init(device, adapter, MB(16))) { return; }
        }
        
        //Describe how many descriptors we want to create
        VkDescriptorPoolSize poolSize = {};
        poolSize.type               = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount    = LAMBDA_SHADERSTAGE_COUNT * LAMBDA_SHADERSTAGE_UNIFORM_COUNT;
        
        //Setup descriptorpool
        VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
        descriptorPoolInfo.sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.flags          = 0;
        descriptorPoolInfo.pNext          = nullptr;
        descriptorPoolInfo.poolSizeCount  = 1;
        descriptorPoolInfo.pPoolSizes     = &poolSize;
        descriptorPoolInfo.maxSets        = LAMBDA_SHADERSTAGE_COUNT;
        
        if (vkCreateDescriptorPool(m_Device, &descriptorPoolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create DescriptorPool\n");
            return;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created DescriptorPool\n");
        }
        
        //Setup allocation of descriptor sets
        VkDescriptorSetAllocateInfo descriptorAllocInfo = {};
        descriptorAllocInfo.sType               = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorAllocInfo.pNext               = nullptr;
        descriptorAllocInfo.descriptorPool      = m_DescriptorPool;
        descriptorAllocInfo.descriptorSetCount  = LAMBDA_SHADERSTAGE_COUNT;
        descriptorAllocInfo.pSetLayouts         = VulkanGraphicsDevice::GetDefaultDescriptorSetLayouts();
        
        //Allocate descriptor sets
        if (vkAllocateDescriptorSets(device, &descriptorAllocInfo, m_DescriptorSets))
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to allocate DescriptorSets\n");
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Allocated DescriptorSets\n");
            
            //Set pipelinelayout to default
            m_PipelineLayout = VulkanGraphicsDevice::GetDefaultPipelineLayout();
            
            //Init descriptors to nulldescriptors
            VkDescriptorBufferInfo descriptors[LAMBDA_SHADERSTAGE_UNIFORM_COUNT];
            for (uint32 i = 0; i < LAMBDA_SHADERSTAGE_UNIFORM_COUNT; i++)
                descriptors[i] =VulkanGraphicsDevice::GetNullBufferDescriptor();
            
            //Write descriptors
            VkWriteDescriptorSet write = {};
            write.sType             = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.pNext             = nullptr;
            write.descriptorCount   = LAMBDA_SHADERSTAGE_UNIFORM_COUNT;
            write.dstArrayElement   = 0;
            write.dstBinding        = 0;
            write.descriptorType    = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write.pBufferInfo       = descriptors;
            write.pImageInfo        = nullptr;
            write.pTexelBufferView  = nullptr;
            
            for (uint32 i = 0; i < LAMBDA_SHADERSTAGE_COUNT; i++)
            {
                write.dstSet = m_DescriptorSets[i];
                vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
            }
        }
    }
    
    
    void VulkanCommandList::Destroy(VkDevice device)
    {
        assert(device != VK_NULL_HANDLE);
        
        //Destroy uploadbuffer
        m_BufferUpload.Destroy(device);
        
        //Destroy descriptorpool
        if (m_DescriptorPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);
            m_DescriptorPool = VK_NULL_HANDLE;
        }
        
        //Free commandbuffers
        if (m_CommandBuffer != VK_NULL_HANDLE)
        {
            vkFreeCommandBuffers(device, m_CommandPool, 1, &m_CommandBuffer);
            m_CommandBuffer = VK_NULL_HANDLE;
        }
        
        //Destroy commandpool
        if (m_CommandPool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(device, m_CommandPool, nullptr);
            m_CommandPool = VK_NULL_HANDLE;
        }
        
        LOG_DEBUG_INFO("Vulkan: Destroyed CommandList '%s'\n", m_Name.c_str());
        
        //Delete me
        delete this;
    }
    
    
    void VulkanCommandList::ClearRenderTarget(ITexture2D* pRenderTarget, float color[4])
    {
        //Set current clear color
        m_ClearColor.float32[0] = color[0];
        m_ClearColor.float32[1] = color[1];
        m_ClearColor.float32[2] = color[2];
        m_ClearColor.float32[3] = color[3];
        
        //TODO: Actually clear image
    }
    
    
    void VulkanCommandList::ClearDepthStencil(ITexture2D* pDepthStencil, float depth, uint8 stencil)
    {
    }
    
    
    void VulkanCommandList::SetRenderTarget(ITexture2D* pRenderTarget, ITexture2D* pDepthStencil)
    {
        m_pRT = pRenderTarget;
        m_pDS = pDepthStencil;
    }
    
    
    void VulkanCommandList::SetViewport(const Viewport& viewport)
    {
        //Setup vulkan viewport
        VkViewport view = {};
        view.width      = viewport.Width;
        view.height     = viewport.Height;
        view.minDepth   = viewport.MinDepth;
        view.maxDepth   = viewport.MaxDepth;
        view.x          = viewport.TopX;
        view.y          = viewport.TopY;
        
        vkCmdSetViewport(m_CommandBuffer, 0, 1, &view);
    }
    
    
    void VulkanCommandList::SetScissorRect(const Math::Rectangle& scissorRect)
    {
        //Setup vulkan scissorrect
        VkRect2D rect = {};
        rect.extent.height  = scissorRect.BottomRight.y;
        rect.extent.width   = scissorRect.BottomRight.x;
        rect.offset.x       = 0;
        rect.offset.y       = 0;
        
        vkCmdSetScissor(m_CommandBuffer, 0, 1, &rect);
    }
    
    
    void VulkanCommandList::SetGraphicsPipelineState(IGraphicsPipelineState* pPSO)
    {
        VkPipeline pipeline = reinterpret_cast<VkPipeline>(pPSO->GetNativeHandle());
        m_RenderPass = reinterpret_cast<VulkanGraphicsPipelineState*>(pPSO)->GetRenderPass();
        
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
        VkBuffer buffer = reinterpret_cast<VkBuffer>(pBuffer->GetNativeHandle());
        //Force uint32 for indices
        vkCmdBindIndexBuffer(m_CommandBuffer, buffer, 0, VK_INDEX_TYPE_UINT32);
    }

    
    CommandListType VulkanCommandList::GetType() const
    {
        return m_Type;
    }
    
    
    void* VulkanCommandList::GetNativeHandle() const
    {
        return (void*)m_CommandBuffer;
    }
    
    
    void VulkanCommandList::TransitionResource(IBuffer* pResource, ResourceState resourceState)
    {
    }
    
    
    void VulkanCommandList::TransitionResource(ITexture2D* pResource, ResourceState resourceState)
    {
    }
    
    
    void VulkanCommandList::VSSetConstantBuffers(const IBuffer* const* ppBuffers, uint32 numBuffers, uint32 startSlot)
    {
    }
    
    
    void VulkanCommandList::VSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot)
    {
    }
    
    
    void VulkanCommandList::VSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot)
    {
    }
    
    
    void VulkanCommandList::HSSetConstantBuffers(const IBuffer* const* ppBuffers, uint32 numBuffers, uint32 startSlot)
    {
    }
    
    
    void VulkanCommandList::HSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot)
    {
    }
    
    
    void VulkanCommandList::HSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot)
    {
    }
    
    
    void VulkanCommandList::DSSetConstantBuffers(const IBuffer* const* ppBuffers, uint32 numBuffers, uint32 startSlot)
    {
    }
    
    
    void VulkanCommandList::DSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot)
    {
    }
    
    
    void VulkanCommandList::DSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot)
    {
    }
    
    
    void VulkanCommandList::GSSetConstantBuffers(const IBuffer* const* ppBuffers, uint32 numBuffers, uint32 startSlot)
    {
    }
    
    
    void VulkanCommandList::GSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot)
    {
    }
    
    
    void VulkanCommandList::GSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot)
    {
    }
    
    
    void VulkanCommandList::PSSetConstantBuffers(const IBuffer* const* ppBuffers, uint32 numBuffers, uint32 startSlot)
    {
        //Set the buffers
        for (uint32 i = 0; i < numBuffers; i++)
        {
            m_ShaderSages[LAMBDA_SHADERSTAGE_PIXEL].UBInfos[startSlot + i].buffer = reinterpret_cast<VkBuffer>(ppBuffers[i]->GetNativeHandle());
            m_ShaderSages[LAMBDA_SHADERSTAGE_PIXEL].UBInfos[startSlot + i].offset = 0;
            m_ShaderSages[LAMBDA_SHADERSTAGE_PIXEL].UBInfos[startSlot + i].range = ppBuffers[i]->GetSizeInBytes();
        }
        
        //Setup write
        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = m_DescriptorSets[LAMBDA_SHADERSTAGE_PIXEL];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = numBuffers;
        descriptorWrite.pBufferInfo = m_ShaderSages[LAMBDA_SHADERSTAGE_PIXEL].UBInfos + startSlot;
        descriptorWrite.pImageInfo = nullptr;
        descriptorWrite.pTexelBufferView = nullptr;
        
        //Update descriptors
        vkUpdateDescriptorSets(m_Device, 1, &descriptorWrite, 0, nullptr);
    }
    
    
    void VulkanCommandList::PSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot)
    {
    }
    
    
    void VulkanCommandList::PSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot)
    {
    }
    
    
    void VulkanCommandList::UpdateBuffer(IBuffer* pResource, const ResourceData* pData)
    {
        //Get offset before allocating
        uint64 offset = m_BufferUpload.GetOffset();
        
        //Allocate memory in the uploadbuffer
        void* pMem = m_BufferUpload.Allocate(pData->SizeInBytes);
        
        //Copy data to GPU
        memcpy(pMem, pData->pData, pData->SizeInBytes);
        
        //Copy buffer
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = offset;
        copyRegion.dstOffset = 0;
        copyRegion.size = pData->SizeInBytes;
        
        vkCmdCopyBuffer(m_CommandBuffer, m_BufferUpload.GetBuffer(), reinterpret_cast<VkBuffer>(pResource->GetNativeHandle()), 1, &copyRegion);
    }
    
    
    void VulkanCommandList::UpdateTexture(ITexture2D* pResource, const ResourceData* pData, uint32 subresource)
    {
    }
    
    
    void VulkanCommandList::CopyBuffer(IBuffer* pDst, IBuffer* pSrc)
    {
        //Copy buffer
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = pDst->GetDesc().SizeInBytes;
        
        vkCmdCopyBuffer(m_CommandBuffer, reinterpret_cast<VkBuffer>(pSrc->GetNativeHandle()), reinterpret_cast<VkBuffer>(pDst->GetNativeHandle()), 1, &copyRegion);
    }
    
    
    void VulkanCommandList::DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation)
    {
        VkClearValue clearColor;
        clearColor.color = m_ClearColor;
        
        VkRenderPassBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.renderPass = m_RenderPass;
        info.framebuffer = VulkanFramebufferCache::GetFramebuffer(m_Device, m_RenderPass, &m_pRT, 1, nullptr);
        info.renderArea.offset = { 0, 0 };
        info.renderArea.extent.width = m_pRT->GetDesc().Width;
        info.renderArea.extent.height = m_pRT->GetDesc().Height;
        info.clearValueCount = 1;
        info.pClearValues = &clearColor;
        
        vkCmdBeginRenderPass(m_CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
        
        vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, LAMBDA_SHADERSTAGE_COUNT, m_DescriptorSets, 0, nullptr);
        vkCmdDraw(m_CommandBuffer, vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
        
        vkCmdEndRenderPass(m_CommandBuffer);
    }
    
    
    void VulkanCommandList::DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation)
    {
        VkClearValue clearColor;
        clearColor.color = m_ClearColor;
        
        VkRenderPassBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.renderPass = m_RenderPass;
        info.framebuffer = VulkanFramebufferCache::GetFramebuffer(m_Device, m_RenderPass, &m_pRT, 1, nullptr);
        info.renderArea.offset = { 0, 0 };
        info.renderArea.extent.width = m_pRT->GetDesc().Width;
        info.renderArea.extent.height = m_pRT->GetDesc().Height;
        info.clearValueCount = 1;
        info.pClearValues = &clearColor;
        
        vkCmdBeginRenderPass(m_CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
        
        vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, LAMBDA_SHADERSTAGE_COUNT, m_DescriptorSets, 0, nullptr);
        vkCmdDrawIndexed(m_CommandBuffer, indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
        
        vkCmdEndRenderPass(m_CommandBuffer);
    }
    
    
    void VulkanCommandList::SetName(const char* pName)
    {
        m_Name = std::string(pName);
    }
    
    
    void VulkanCommandList::Close()
    {
        //End and close commandbuffer
        if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to End CommandBuffer\n");
        }
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
        
        //Reset uploadbuffer
        m_BufferUpload.Reset();
    }
}
