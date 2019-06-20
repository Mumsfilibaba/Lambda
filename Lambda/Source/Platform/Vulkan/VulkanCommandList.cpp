#include "LambdaPch.h"
#include "VulkanCommandList.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanPipelineState.h"
#include "VulkanTexture2D.h"
#include "VulkanFramebufferCache.h"

namespace Lambda
{
    VulkanCommandList::VulkanCommandList(VkDevice device, CommandListType type)
        : m_CommandPool(VK_NULL_HANDLE),
        m_CommandBuffer(VK_NULL_HANDLE),
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
        }
    }
    
    
    void VulkanCommandList::Destroy(VkDevice device)
    {
        assert(device != VK_NULL_HANDLE);
        
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
    }
    
    
    void VulkanCommandList::ClearDepthStencil(ITexture2D* pDepthStencil, float depth, uint8 stencil)
    {
    }
    
    
    void VulkanCommandList::SetRenderTarget(ITexture2D* pRenderTarget, ITexture2D* pDepthStencil)
    {
        m_pRT = pRenderTarget;
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
    
    
    void VulkanCommandList::SetPrimtiveTopology(PrimitiveTopology topology)
    {
    }
    
    
    void VulkanCommandList::SetGraphicsPipelineState(IGraphicsPipelineState* pPSO)
    {
        VulkanGraphicsPipelineState* pPipeline = reinterpret_cast<VulkanGraphicsPipelineState*>(pPSO);
        m_RenderPass = pPipeline->GetRenderPass();
        
        vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->GetPipeline());
    }
    
    
    void VulkanCommandList::SetVertexBuffer(IBuffer* pBuffer, uint32 slot)
    {
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
    }
    
    
    void VulkanCommandList::PSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot)
    {
    }
    
    
    void VulkanCommandList::PSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot)
    {
    }
    
    
    void VulkanCommandList::UpdateBuffer(IBuffer* pResource, const ResourceData* pData)
    {
    }
    
    
    void VulkanCommandList::UpdateTexture(ITexture2D* pResource, const ResourceData* pData, uint32 subresource)
    {
    }
    
    
    void VulkanCommandList::CopyBuffer(IBuffer* pDst, IBuffer* pSrc)
    {
    }
    
    
    void VulkanCommandList::DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation)
    {
        VkDevice device = reinterpret_cast<VkDevice>(IGraphicsDevice::GetInstance()->GetNativeHandle());
        VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
        
        VkRenderPassBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.renderPass = m_RenderPass;
        info.framebuffer = VulkanFramebufferCache::GetFramebuffer(device, m_RenderPass, &m_pRT, 1, nullptr);
        info.renderArea.offset = { 0, 0 };
        info.renderArea.extent.width = m_pRT->GetDesc().Width;
        info.renderArea.extent.height = m_pRT->GetDesc().Height;
        info.clearValueCount = 1;
        info.pClearValues = &clearColor;
        
        vkCmdBeginRenderPass(m_CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdDraw(m_CommandBuffer, vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
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
        if (vkResetCommandBuffer(m_CommandBuffer, 0) != VK_SUCCESS)
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
    }
}