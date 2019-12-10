#pragma once
#include "Graphics/Core/CShaderBase.h"
#include <vector>
#include <string>
#include "Vulkan.h"

namespace Lambda
{
	class VKNDevice;

	//---------------
	//VKNSamplerState
	//---------------

    class VKNShader final : public CShaderBase<VKNDevice>
    {
    public:
        VKNShader(VKNDevice* pVkDevice, const SShaderDesc& desc);
        ~VKNShader();

        LAMBDA_NO_COPY(VKNShader);
        
        virtual void* GetNativeHandle() const override final;

		_forceinline VkShaderModule GetVkShaderModule() const 
		{ 
			return m_VkShaderModule; 
		}
    private:
        void Init(const SShaderDesc& desc);
    private:
        std::vector<char> m_ByteCode;
        std::string m_EntryPoint;
        VkShaderModule m_VkShaderModule;
    };
}
