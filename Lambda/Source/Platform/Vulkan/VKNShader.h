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
        
        VKNShader(VKNDevice* pDevice, const ShaderDesc& desc);
        ~VKNShader();
        
        virtual void* GetNativeHandle() const override final;

		inline VkShaderModule GetVkShaderModule() const { return m_Shader; }
    private:
        void Init(const ShaderDesc& desc);
    private:
        VkShaderModule	  m_Shader;
        std::string		  m_EntryPoint;
        std::vector<char> m_ByteCode;
    };
}
