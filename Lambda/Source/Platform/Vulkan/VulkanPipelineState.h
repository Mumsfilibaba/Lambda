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
        
    private:
        void Init(VkDevice device, const GraphicsPipelineStateDesc& desc);
        
    private:
    };
}
