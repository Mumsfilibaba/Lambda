#pragma once
#include "Graphics/IShader.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <string>

namespace Lambda
{
	//---------------
	//VKNSamplerState
	//---------------

    class VKNShader final : public IShader
    {
    public:
        LAMBDA_NO_COPY(VKNShader);
        
        VKNShader(const ShaderDesc& desc);
        ~VKNShader() = default;
        
        virtual void*		GetNativeHandle() const override final;
		virtual ShaderDesc	GetDesc() const override final;
        
        void Destroy(VkDevice device);
        
    private:
        void Init(const ShaderDesc& desc);
        
    private:
        VkShaderModule		m_Shader;
        std::vector<char>	m_ByteCode;
        std::string			m_EntryPoint;
		ShaderDesc			m_Desc;
    };
}
