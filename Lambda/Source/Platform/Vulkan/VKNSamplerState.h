#pragma once
#include "Graphics/Core/CSamplerStateBase.h"
#include "Vulkan.h"

namespace Lambda
{
	class VKNDevice;

	//---------------
	//VKNSamplerState
	//---------------

    class VKNSamplerState : public CSamplerStateBase<VKNDevice>
    {
		using TSamplerState = CSamplerStateBase<VKNDevice>;
    public:        
        VKNSamplerState(VKNDevice* pVkDevice, const SSamplerStateDesc& desc);
        ~VKNSamplerState();

        LAMBDA_NO_COPY(VKNSamplerState);
        
        virtual void* GetNativeHandle() const override final;
		virtual void SetName(const char* pName) override final;

		_forceinline VkSampler GetVkSampler() const 
		{ 
			return m_VkSampler; 
		}
    private:
        void Init(const SSamplerStateDesc& desc);
    private:
        VkSampler m_VkSampler;
    };
}
