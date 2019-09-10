#pragma once
#include "Graphics/IShader.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <string>

namespace Lambda
{
    class VulkanShader final : public IShader
    {
    public:
        LAMBDA_NO_COPY(VulkanShader);
        
        VulkanShader(VkDevice device, const ShaderDesc& desc);
        ~VulkanShader() = default;
        
        virtual void* GetNativeHandle() const override final;
		virtual ShaderDesc GetDesc() const override final;
        
        void Destroy(VkDevice device);
        
    private:
        void Init(VkDevice device, const ShaderDesc& desc);
        
    private:
        VkShaderModule m_Shader;
        std::vector<char> m_ByteCode;
        std::string m_EntryPoint;
		ShaderDesc m_Desc;
    };
}
