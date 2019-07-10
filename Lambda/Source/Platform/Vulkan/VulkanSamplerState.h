#pragma once
#include "Graphics/ISamplerState.h"
#include <vulkan/vulkan.h>

namespace Lambda
{
    class VulkanSamplerState : public ISamplerState
    {
    public:
        LAMBDA_NO_COPY(VulkanSamplerState);
        
        VulkanSamplerState(VkDevice device, const SamplerDesc& desc);
        ~VulkanSamplerState() = default;
        
        virtual SamplerDesc GetDesc() const override final;
        virtual void* GetNativeHandle() const override final;
        
        void Destroy(VkDevice device);
        
    private:
        void Init(VkDevice device, const SamplerDesc& desc);
        
    private:
        VkSampler m_Sampler;
        SamplerDesc m_Desc;
    };
}
