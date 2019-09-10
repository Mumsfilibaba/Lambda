#pragma once
#include "Graphics/ISamplerState.h"
#include <vulkan/vulkan.h>

namespace Lambda
{
    class VulkanSamplerState : public ISamplerState
    {
    public:
        LAMBDA_NO_COPY(VulkanSamplerState);
        
        VulkanSamplerState(VkDevice device, const SamplerStateDesc& desc);
        ~VulkanSamplerState() = default;
        
        virtual void* GetNativeHandle() const override final;
        virtual SamplerStateDesc GetDesc() const override final;
        
        void Destroy(VkDevice device);
        
    private:
        void Init(VkDevice device, const SamplerStateDesc& desc);
        
    private:
        VkSampler m_Sampler;
		SamplerStateDesc m_Desc;
    };
}
