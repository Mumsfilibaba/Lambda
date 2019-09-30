#pragma once
#include "Graphics/IPipelineState.h"
#include <vulkan/vulkan.h>

namespace Lambda
{
	class VKNDevice;

	//------------------------
	//VKNGraphicsPipelineState
	//------------------------

    class VKNGraphicsPipelineState final : public RefCountedObject<IGraphicsPipelineState>
    {
    public:
        LAMBDA_NO_COPY(VKNGraphicsPipelineState);
        
        VKNGraphicsPipelineState(VKNDevice* pDevice, const GraphicsPipelineStateDesc& desc);
        ~VKNGraphicsPipelineState();
        
        virtual void SetName(const char* pName) override final;
        virtual void* GetNativeHandle() const override final;
    private:
        void Init(const GraphicsPipelineStateDesc& desc);        
    private:
		VKNDevice* m_pDevice;
        VkPipeline m_Pipeline;
    };
}
