#pragma once
#include "Graphics/IPipelineState.h"
#include <vulkan/vulkan.h>

namespace Lambda
{
	//------------------------
	//VKNGraphicsPipelineState
	//------------------------

    class VKNGraphicsPipelineState final : public IGraphicsPipelineState
    {
    public:
        LAMBDA_NO_COPY(VKNGraphicsPipelineState);
        
        VKNGraphicsPipelineState(const GraphicsPipelineStateDesc& desc);
        VKNGraphicsPipelineState() = default;
        
        virtual void SetName(const char* pName) override final;
        virtual void* GetNativeHandle() const override final;
        
        void Destroy(VkDevice device);
        
    private:
        void Init(const GraphicsPipelineStateDesc& desc);
        
    private:
        VkPipeline m_Pipeline;
    };
}
