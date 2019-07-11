#include "LambdaPch.h"
#include "VulkanCommandList.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanPipelineState.h"
#include "VulkanSamplerState.h"
#include "VulkanTexture2D.h"
#include "VulkanFramebufferCache.h"
#include "VulkanBuffer.h"
#include "VulkanConversions.inl"

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
        //Init samplers in texturedescriptors to null
        for (uint32 i = 0; i < LAMBDA_SHADERSTAGE_COUNT; i++)
        {
            for (uint32 j = 0; j < LAMBDA_SHADERSTAGE_TEXTURE_COUNT; j++)
                m_ShaderSages[i].TextureInfos[j].sampler = VK_NULL_HANDLE;
        }
        
        //Init imageview in samplerdescriptors to null
        for (uint32 i = 0; i < LAMBDA_SHADERSTAGE_COUNT; i++)
        {
            for (uint32 j = 0; j < LAMBDA_SHADERSTAGE_SAMPLER_COUNT; j++)
            {
                m_ShaderSages[i].SamplerInfos[j].imageLayout    = VK_IMAGE_LAYOUT_UNDEFINED;
                m_ShaderSages[i].SamplerInfos[j].imageView      = VK_NULL_HANDLE;
            }
        }

        
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
            if (!m_TextureUpload.Init(device, adapter, MB(32))) { return; }
        }
        
        //Describe how many descriptors we want to create
        VkDescriptorPoolSize poolSizes[3] = {};
        poolSizes[0].type               = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount    = LAMBDA_SHADERSTAGE_COUNT * LAMBDA_SHADERSTAGE_UNIFORM_COUNT;
        
        poolSizes[1].type               = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        poolSizes[1].descriptorCount    = LAMBDA_SHADERSTAGE_COUNT * LAMBDA_SHADERSTAGE_TEXTURE_COUNT;
        
        poolSizes[2].type               = VK_DESCRIPTOR_TYPE_SAMPLER;
        poolSizes[2].descriptorCount    = LAMBDA_SHADERSTAGE_COUNT * LAMBDA_SHADERSTAGE_SAMPLER_COUNT;
        
        //Setup descriptorpool
        VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
        descriptorPoolInfo.sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.flags          = 0;
        descriptorPoolInfo.pNext          = nullptr;
        descriptorPoolInfo.poolSizeCount  = 3;
        descriptorPoolInfo.pPoolSizes     = poolSizes;
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
            VkDescriptorBufferInfo bufferDescriptors[LAMBDA_SHADERSTAGE_UNIFORM_COUNT];
            for (uint32 i = 0; i < LAMBDA_SHADERSTAGE_UNIFORM_COUNT; i++)
                bufferDescriptors[i] = VulkanGraphicsDevice::GetNullBufferDescriptor();
            
            VkDescriptorImageInfo textureDescriptors[LAMBDA_SHADERSTAGE_TEXTURE_COUNT];
            for (uint32 i = 0; i < LAMBDA_SHADERSTAGE_TEXTURE_COUNT; i++)
                textureDescriptors[i] = VulkanGraphicsDevice::GetNullTextureDescriptor();
            
            VkDescriptorImageInfo samplerDescriptors[LAMBDA_SHADERSTAGE_SAMPLER_COUNT];
            for (uint32 i = 0; i < LAMBDA_SHADERSTAGE_SAMPLER_COUNT; i++)
                samplerDescriptors[i] = VulkanGraphicsDevice::GetNullSamplerDescriptor();
            
            //Vector for all writes
            std::vector<VkWriteDescriptorSet> descriptorWrites;
            
            //Setup bufferdescriptors
            VkWriteDescriptorSet bufferWrite = {};
            bufferWrite.sType             = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            bufferWrite.pNext             = nullptr;
            bufferWrite.descriptorCount   = LAMBDA_SHADERSTAGE_UNIFORM_COUNT;
            bufferWrite.dstArrayElement   = 0;
            bufferWrite.dstBinding        = VulkanGraphicsDevice::GetUniformBinding();
            bufferWrite.descriptorType    = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            bufferWrite.pBufferInfo       = bufferDescriptors;
            bufferWrite.pImageInfo        = nullptr;
            bufferWrite.pTexelBufferView  = nullptr;
            
            //Setup texturedescriptors
            VkWriteDescriptorSet textureWrite = {};
            textureWrite.sType             = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            textureWrite.pNext             = nullptr;
            textureWrite.descriptorCount   = LAMBDA_SHADERSTAGE_TEXTURE_COUNT;
            textureWrite.dstArrayElement   = 0;
            textureWrite.dstBinding        = VulkanGraphicsDevice::GetTextureBinding();
            textureWrite.descriptorType    = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            textureWrite.pBufferInfo       = nullptr;
            textureWrite.pImageInfo        = textureDescriptors;
            textureWrite.pTexelBufferView  = nullptr;
            
            //Setup samplerdescriptors
            VkWriteDescriptorSet samplerWrite = {};
            samplerWrite.sType             = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            samplerWrite.pNext             = nullptr;
            samplerWrite.descriptorCount   = LAMBDA_SHADERSTAGE_SAMPLER_COUNT;
            samplerWrite.dstArrayElement   = 0;
            samplerWrite.dstBinding        = VulkanGraphicsDevice::GetSamplerBinding();
            samplerWrite.descriptorType    = VK_DESCRIPTOR_TYPE_SAMPLER;
            samplerWrite.pBufferInfo       = nullptr;
            samplerWrite.pImageInfo        = samplerDescriptors;
            samplerWrite.pTexelBufferView  = nullptr;
            
            for (uint32 i = 0; i < LAMBDA_SHADERSTAGE_COUNT; i++)
            {
                bufferWrite.dstSet = m_DescriptorSets[i];
                descriptorWrites.push_back(bufferWrite);
                
                textureWrite.dstSet = m_DescriptorSets[i];
                descriptorWrites.push_back(textureWrite);
                
                samplerWrite.dstSet = m_DescriptorSets[i];
                descriptorWrites.push_back(samplerWrite);
            }
            
            //Write descriptors
            vkUpdateDescriptorSets(device, uint32(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }
    
    
    void VulkanCommandList::InternalWriteConstantBufferDescriptorsToStage(uint32 shaderStage, uint32 startSlot, const IBuffer* const* ppBuffers, uint32 numBuffers)
    {
        //Set the buffers
        for (uint32 i = 0; i < numBuffers; i++)
        {
            m_ShaderSages[shaderStage].UBInfos[startSlot + i].buffer   = reinterpret_cast<VkBuffer>(ppBuffers[i]->GetNativeHandle());
            m_ShaderSages[shaderStage].UBInfos[startSlot + i].offset   = 0;
            m_ShaderSages[shaderStage].UBInfos[startSlot + i].range    = ppBuffers[i]->GetSizeInBytes();
        }
        
        //Setup write
        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType               = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet              = m_DescriptorSets[shaderStage];
        descriptorWrite.dstBinding          = VulkanGraphicsDevice::GetUniformBinding();
        descriptorWrite.dstArrayElement     = 0;
        descriptorWrite.descriptorType      = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount     = numBuffers;
        descriptorWrite.pBufferInfo         = m_ShaderSages[shaderStage].UBInfos + startSlot;
        descriptorWrite.pImageInfo          = nullptr;
        descriptorWrite.pTexelBufferView    = nullptr;
        
        //Update descriptors
        vkUpdateDescriptorSets(m_Device, 1, &descriptorWrite, 0, nullptr);
    }
    
    
    void VulkanCommandList::InternalWriteTextureDescriptorsToStage(uint32 shaderStage, uint32 startSlot, const ITexture2D* const* ppTextures, uint32 numTextures)
    {
        //Set the textures
        for (uint32 i = 0; i < numTextures; i++)
        {
            m_ShaderSages[shaderStage].TextureInfos[startSlot + i].imageLayout  = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            m_ShaderSages[shaderStage].TextureInfos[startSlot + i].imageView    = reinterpret_cast<const VulkanTexture2D*>(ppTextures[i])->GetImageView();
        }
        
        //Setup write
        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType               = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet              = m_DescriptorSets[shaderStage];
        descriptorWrite.dstBinding          = VulkanGraphicsDevice::GetTextureBinding();
        descriptorWrite.dstArrayElement     = 0;
        descriptorWrite.descriptorType      = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        descriptorWrite.descriptorCount     = numTextures;
        descriptorWrite.pBufferInfo         = nullptr;
        descriptorWrite.pImageInfo          = m_ShaderSages[shaderStage].TextureInfos + startSlot;
        descriptorWrite.pTexelBufferView    = nullptr;
        
        //Update descriptors
        vkUpdateDescriptorSets(m_Device, 1, &descriptorWrite, 0, nullptr);
    }
    
    
    void VulkanCommandList::InternalWriteSamplerDescriptorsToStage(uint32 shaderStage, uint32 startSlot, const ISamplerState* const* ppSamplers, uint32 numSamplers)
    {
        //Set the numSamplers
        for (uint32 i = 0; i < numSamplers; i++)
            m_ShaderSages[shaderStage].SamplerInfos[startSlot + i].sampler = reinterpret_cast<VkSampler>(ppSamplers[i]->GetNativeHandle());
        
        //Setup write
        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType               = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet              = m_DescriptorSets[shaderStage];
        descriptorWrite.dstBinding          = VulkanGraphicsDevice::GetSamplerBinding();
        descriptorWrite.dstArrayElement     = 0;
        descriptorWrite.descriptorType      = VK_DESCRIPTOR_TYPE_SAMPLER;
        descriptorWrite.descriptorCount     = numSamplers;
        descriptorWrite.pBufferInfo         = nullptr;
        descriptorWrite.pImageInfo          = m_ShaderSages[shaderStage].SamplerInfos + startSlot;
        descriptorWrite.pTexelBufferView    = nullptr;
        
        //Update descriptors
        vkUpdateDescriptorSets(m_Device, 1, &descriptorWrite, 0, nullptr);
    }
    
    
    void VulkanCommandList::Destroy(VkDevice device)
    {
        assert(device != VK_NULL_HANDLE);
        
        //Destroy uploadbuffer
        m_BufferUpload.Destroy(device);
        m_TextureUpload.Destroy(device);
        
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
    
    
    void VulkanCommandList::TransitionBuffer(IBuffer* pBuffer, ResourceState resourceState)
    {
    }
    
    
    void VulkanCommandList::TransitionTexture(ITexture2D* pTexture, ResourceState resourceState)
    {
        //Get image
        VkImage image = reinterpret_cast<VkImage>(pTexture->GetNativeHandle());
        
        //Setup barrier
        VkImageMemoryBarrier barrier = {};
        barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout                       = reinterpret_cast<VulkanTexture2D*>(pTexture)->GetCurrentResourceState();
        barrier.newLayout                       = ConvertResourceStateToImageLayout(resourceState);
        barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.image                           = image;
        barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;
        barrier.srcAccessMask                   = 0; // TODO
        barrier.dstAccessMask                   = 0; // TODO
        
        //Set flags and stage
        VkPipelineStageFlags sourceStage        = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkPipelineStageFlags destinationStage   = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        
        if (barrier.oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && barrier.newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (barrier.oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && barrier.newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else
        {
            LOG_DEBUG_ERROR("Vulkan: Unsupported layout transition\n");
        }
        
        
        //Move image to correct layout (Resource state)
        vkCmdPipelineBarrier(m_CommandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        
        //Set the new state to the texture
        reinterpret_cast<VulkanTexture2D*>(pTexture)->SetCurrentResourceState(barrier.newLayout);
    }
    
    
    void VulkanCommandList::VSSetConstantBuffers(const IBuffer* const* ppBuffers, uint32 numBuffers, uint32 startSlot)
    {
        InternalWriteConstantBufferDescriptorsToStage(LAMBDA_SHADERSTAGE_VERTEX, startSlot, ppBuffers, numBuffers);
    }
    
    
    void VulkanCommandList::VSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot)
    {
         InternalWriteTextureDescriptorsToStage(LAMBDA_SHADERSTAGE_VERTEX, startSlot, ppTextures, numTextures);
    }
    
    
    void VulkanCommandList::VSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot)
    {
        InternalWriteSamplerDescriptorsToStage(LAMBDA_SHADERSTAGE_VERTEX, startSlot, ppSamplerStates, numSamplers);
    }
    
    
    void VulkanCommandList::HSSetConstantBuffers(const IBuffer* const* ppBuffers, uint32 numBuffers, uint32 startSlot)
    {
         InternalWriteConstantBufferDescriptorsToStage(LAMBDA_SHADERSTAGE_HULL, startSlot, ppBuffers, numBuffers);
    }
    
    
    void VulkanCommandList::HSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot)
    {
         InternalWriteTextureDescriptorsToStage(LAMBDA_SHADERSTAGE_HULL, startSlot, ppTextures, numTextures);
    }
    
    
    void VulkanCommandList::HSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot)
    {
        InternalWriteSamplerDescriptorsToStage(LAMBDA_SHADERSTAGE_HULL, startSlot, ppSamplerStates, numSamplers);
    }
    
    
    void VulkanCommandList::DSSetConstantBuffers(const IBuffer* const* ppBuffers, uint32 numBuffers, uint32 startSlot)
    {
         InternalWriteConstantBufferDescriptorsToStage(LAMBDA_SHADERSTAGE_DOMAIN, startSlot, ppBuffers, numBuffers);
    }
    
    
    void VulkanCommandList::DSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot)
    {
         InternalWriteTextureDescriptorsToStage(LAMBDA_SHADERSTAGE_DOMAIN, startSlot, ppTextures, numTextures);
    }
    
    
    void VulkanCommandList::DSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot)
    {
        InternalWriteSamplerDescriptorsToStage(LAMBDA_SHADERSTAGE_DOMAIN, startSlot, ppSamplerStates, numSamplers);
    }
    
    
    void VulkanCommandList::GSSetConstantBuffers(const IBuffer* const* ppBuffers, uint32 numBuffers, uint32 startSlot)
    {
         InternalWriteConstantBufferDescriptorsToStage(LAMBDA_SHADERSTAGE_GEOMETRY, startSlot, ppBuffers, numBuffers);
    }
    
    
    void VulkanCommandList::GSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot)
    {
         InternalWriteTextureDescriptorsToStage(LAMBDA_SHADERSTAGE_GEOMETRY, startSlot, ppTextures, numTextures);
    }
    
    
    void VulkanCommandList::GSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot)
    {
        InternalWriteSamplerDescriptorsToStage(LAMBDA_SHADERSTAGE_GEOMETRY, startSlot, ppSamplerStates, numSamplers);
    }
    
    
    void VulkanCommandList::PSSetConstantBuffers(const IBuffer* const* ppBuffers, uint32 numBuffers, uint32 startSlot)
    {
        InternalWriteConstantBufferDescriptorsToStage(LAMBDA_SHADERSTAGE_PIXEL, startSlot, ppBuffers, numBuffers);
    }
    
    
    void VulkanCommandList::PSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot)
    {
        InternalWriteTextureDescriptorsToStage(LAMBDA_SHADERSTAGE_PIXEL, startSlot, ppTextures, numTextures);
    }
    
    
    void VulkanCommandList::PSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot)
    {
        InternalWriteSamplerDescriptorsToStage(LAMBDA_SHADERSTAGE_PIXEL, startSlot, ppSamplerStates, numSamplers);
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
        //Get offset before allocating
        uint64 offset = m_TextureUpload.GetOffset();
        
        //Allocate memory in the uploadbuffer
        void* pMem = m_TextureUpload.Allocate(pData->SizeInBytes);
        
        //Copy data to GPU
        memcpy(pMem, pData->pData, pData->SizeInBytes);
        
        //Setup copy buffer
        VkBufferImageCopy region = {};
        region.bufferOffset                     = offset;
        region.bufferRowLength                  = 0;
        region.bufferImageHeight                = 0;
        region.imageSubresource.aspectMask      = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel        = 0;
        region.imageSubresource.baseArrayLayer  = 0;
        region.imageSubresource.layerCount      = 1;
        region.imageOffset                      = { 0, 0, 0 };
        region.imageExtent                      = { pResource->GetWidth(), pResource->GetHeight(), 1 };
        
        //Get native resources
        VkBuffer buffer = m_TextureUpload.GetBuffer();
        VkImage image = reinterpret_cast<VkImage>(pResource->GetNativeHandle());
        
        //Copy buffer to image
        vkCmdCopyBufferToImage(m_CommandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
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
        
        //Begin renderpass
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
        
        //Set descriptorsets
        vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, LAMBDA_SHADERSTAGE_COUNT, m_DescriptorSets, 0, nullptr);
        
        //Draw current vertexbuffer
        vkCmdDraw(m_CommandBuffer, vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
        
        //End renderpass
        vkCmdEndRenderPass(m_CommandBuffer);
    }
    
    
    void VulkanCommandList::DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation)
    {
        VkClearValue clearColor;
        clearColor.color = m_ClearColor;
        
        //Begin renderpass
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
        
        //Bind descriptors
        vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, LAMBDA_SHADERSTAGE_COUNT, m_DescriptorSets, 0, nullptr);
        
        //Draw current vertex- and indexbuffer
        vkCmdDrawIndexed(m_CommandBuffer, indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
        
        //End renderpass
        vkCmdEndRenderPass(m_CommandBuffer);
    }
    
    
    void VulkanCommandList::SetName(const char* pName)
    {
        //Set internal name
        m_Name = std::string(pName);
        
        //Set name in vulkan
        VulkanGraphicsDevice::SetVulkanObjectName(VK_OBJECT_TYPE_COMMAND_BUFFER, (uint64)m_CommandBuffer, m_Name);
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
