#pragma once
#include "Graphics/ISamplerState.h"
#include <vulkan/vulkan.h>

namespace Lambda
{
	class VKNDevice;

	//---------------
	//VKNSamplerState
	//---------------

    class VKNSamplerState : public RefCountedObject<ISamplerState>
    {
    public:
        LAMBDA_NO_COPY(VKNSamplerState);
        
        VKNSamplerState(VKNDevice* pDevice, const SamplerStateDesc& desc);
        ~VKNSamplerState();
        
        virtual void* GetNativeHandle() const override final;
        virtual const SamplerStateDesc& GetDesc() const override final;
    private:
        void Init(const SamplerStateDesc& desc);     
    private:
		VKNDevice*			m_pDevice;
        VkSampler			m_Sampler;
		SamplerStateDesc	m_Desc;
    };
}
