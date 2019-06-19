#pragma once
#include "Graphics/IPipelineState.h"
#include <vulkan/vulkan.h>

namespace Lambda
{
    class VulkanGraphicsPipelineState final : public IGraphicsPipelineState
    {
    public:
        LAMBDA_NO_COPY(VulkanGraphicsPipelineState);
        
        VulkanGraphicsPipelineState(VkDevice device, const GraphicsPipelineStateDesc& desc);
        VulkanGraphicsPipelineState() = default;
        
        void Destroy(VkDevice device);
        VkPipeline GetPipeline() const;
        VkRenderPass GetRenderPass() const;
        
    private:
        void Init(VkDevice device, const GraphicsPipelineStateDesc& desc);
        
    private:
        VkPipeline m_Pipeline;
        VkRenderPass m_RenderPass;
        VkPipelineLayout m_PipelineLayout;
    };
    
    
    inline VkPipeline VulkanGraphicsPipelineState::GetPipeline() const
    {
        return m_Pipeline;
    }
    
    
    inline VkRenderPass VulkanGraphicsPipelineState::GetRenderPass() const
    {
        return m_RenderPass;
    }
}
