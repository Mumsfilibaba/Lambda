#pragma once
#include "Graphics/Core/ISamplerState.h"
#include "VKNDeviceObject.h"
#include <vulkan/vulkan.h>

namespace Lambda
{
	class VKNDevice;

	//---------------
	//VKNSamplerState
	//---------------

    class VKNSamplerState : public VKNDeviceObject<ISamplerState>
    {
    public:
        LAMBDA_NO_COPY(VKNSamplerState);
        
        VKNSamplerState(VKNDevice* pDevice, const SamplerStateDesc& desc);
        ~VKNSamplerState();
        
        virtual void* GetNativeHandle() const override final;
        virtual const SamplerStateDesc& GetDesc() const override final;

		inline VkSampler GetVkSampler() const { return m_Sampler; }
    private:
        void Init(const SamplerStateDesc& desc);     
    private:
        VkSampler			m_Sampler;
		SamplerStateDesc	m_Desc;
    };
}
