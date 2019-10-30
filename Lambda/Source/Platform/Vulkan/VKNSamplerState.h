#pragma once
#include "Graphics/Core/SamplerStateBase.h"
#include "Vulkan.h"

namespace Lambda
{
	class VKNDevice;

	//---------------
	//VKNSamplerState
	//---------------

    class VKNSamplerState : public SamplerStateBase<VKNDevice>
    {
		using TSamplerState = SamplerStateBase<VKNDevice>;

    public:
        LAMBDA_NO_COPY(VKNSamplerState);
        
        VKNSamplerState(VKNDevice* pDevice, const SamplerStateDesc& desc);
        ~VKNSamplerState();
        
        virtual void* GetNativeHandle() const override final;
		virtual void SetName(const char* pName) override final;

		_forceinline VkSampler GetVkSampler() const 
		{ 
			return m_Sampler; 
		}
    private:
        void Init(const SamplerStateDesc& desc);     
    private:
        VkSampler m_Sampler;
    };
}
