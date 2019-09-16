#pragma once
#include "Graphics/ISamplerState.h"
#include <vulkan/vulkan.h>

namespace Lambda
{
	//---------------
	//VKNSamplerState
	//---------------

    class VKNSamplerState : public ISamplerState
    {
    public:
        LAMBDA_NO_COPY(VKNSamplerState);
        
        VKNSamplerState(const SamplerStateDesc& desc);
        ~VKNSamplerState() = default;
        
        virtual void*				GetNativeHandle() const override final;
        virtual SamplerStateDesc	GetDesc() const override final;
        
        void Destroy(VkDevice device);
        
    private:
        void Init(const SamplerStateDesc& desc);
        
    private:
        VkSampler			m_Sampler;
		SamplerStateDesc	m_Desc;
    };
}
