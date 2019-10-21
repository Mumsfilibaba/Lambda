#pragma once
#include "Graphics/Core/SamplerStateBase.h"
#include <vulkan/vulkan.h>

namespace Lambda
{
	class VKNDevice;

	//---------------
	//VKNSamplerState
	//---------------

    class VKNSamplerState : public SamplerStateBase<VKNDevice>
    {
    public:
        LAMBDA_NO_COPY(VKNSamplerState);
        
        VKNSamplerState(VKNDevice* pDevice, const SamplerStateDesc& desc);
        ~VKNSamplerState();
        
        virtual void* GetNativeHandle() const override final;

		inline VkSampler GetVkSampler() const { return m_Sampler; }
    private:
        void Init(const SamplerStateDesc& desc);     
    private:
        VkSampler m_Sampler;
    };
}
