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
        
        virtual void SetName(const char* pName) override final;
        virtual void* GetNativeHandle() const override final;
        
        void Destroy(VkDevice device);
        
    private:
        void Init(VkDevice device, const GraphicsPipelineStateDesc& desc);
        
    private:
        VkPipeline m_Pipeline;
    };
}
