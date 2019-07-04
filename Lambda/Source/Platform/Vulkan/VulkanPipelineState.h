#pragma once
#include "Graphics/IPipelineState.h"
#include <vulkan/vulkan.h>

namespace Lambda
{
    //Vulkan implementation of GraphicsPipelineState
    class VulkanGraphicsPipelineState final : public IGraphicsPipelineState
    {
    public:
        LAMBDA_NO_COPY(VulkanGraphicsPipelineState);
        
        VulkanGraphicsPipelineState(VkDevice device, const GraphicsPipelineStateDesc& desc);
        VulkanGraphicsPipelineState() = default;
        
        virtual void* GetNativeHandle() const override final;
        
        void Destroy(VkDevice device);
        VkRenderPass GetRenderPass() const;
        
    private:
        void Init(VkDevice device, const GraphicsPipelineStateDesc& desc);
        
    private:
        VkPipeline m_Pipeline;
        VkRenderPass m_RenderPass;
    };
    
    
    inline VkRenderPass VulkanGraphicsPipelineState::GetRenderPass() const
    {
        return m_RenderPass;
    }
}
