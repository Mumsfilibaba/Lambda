#pragma once
#include "Graphics/Core/IShader.h"
#include "VKNDeviceObject.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <string>

namespace Lambda
{
	class VKNDevice;

	//---------------
	//VKNSamplerState
	//---------------

    class VKNShader final : public VKNDeviceObject<IShader>
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
        VkShaderModule		m_Shader;
        std::vector<char>	m_ByteCode;
        std::string			m_EntryPoint;
		ShaderDesc			m_Desc;
    };
}
