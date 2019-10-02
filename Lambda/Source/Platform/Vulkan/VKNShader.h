#pragma once
#include "Graphics/Core/IShader.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <string>

namespace Lambda
{
	class VKNDevice;

	//---------------
	//VKNSamplerState
	//---------------

    class VKNShader final : public RefCountedObject<IShader>
    {
    public:
        LAMBDA_NO_COPY(VKNShader);
        
        VKNShader(VKNDevice* pDevice, const ShaderDesc& desc);
        ~VKNShader();
        
        virtual void* GetNativeHandle() const override final;
		virtual const ShaderDesc& GetDesc() const override final;

		inline VkShaderModule GetVkShaderModule() const { return m_Shader; }
    private:
        void Init(const ShaderDesc& desc);
    private:
		VKNDevice*			m_pDevice;
        VkShaderModule		m_Shader;
        std::vector<char>	m_ByteCode;
        std::string			m_EntryPoint;
		ShaderDesc			m_Desc;
    };
}
