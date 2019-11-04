#pragma once
#include "Graphics/Core/ShaderBase.h"
#include <vector>
#include <string>
#include "Vulkan.h"

namespace Lambda
{
	class VKNDevice;

	//---------------
	//VKNSamplerState
	//---------------

    class VKNShader final : public ShaderBase<VKNDevice>
    {
    public:
        LAMBDA_NO_COPY(VKNShader);
        
        VKNShader(VKNDevice* pVkDevice, const ShaderDesc& desc);
        ~VKNShader();
        
        virtual void* GetNativeHandle() const override final;

		_forceinline VkShaderModule GetVkShaderModule() const 
		{ 
			return m_VkShaderModule; 
		}
    private:
        void Init(const ShaderDesc& desc);
    private:
        std::vector<char> m_ByteCode;
        std::string m_EntryPoint;
        VkShaderModule m_VkShaderModule;
    };
}
