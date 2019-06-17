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
        
        void Destroy(VkDevice device);
        virtual ShaderType GetType() const override final;
        
        const char* GetEntryPoint() const;
        VkShaderModule GetShaderModule() const;
        
    private:
        void Init(VkDevice device, const ShaderDesc& desc);
        
    private:
        VkShaderModule m_Shader;
        std::vector<const char> m_ByteCode;
        std::string m_EntryPoint;
        ShaderType m_Type;
    };
    
    
    inline const char* VulkanShader::GetEntryPoint() const
    {
        return m_EntryPoint.c_str();
    }
    
    
    inline VkShaderModule VulkanShader::GetShaderModule() const
    {
        return m_Shader;
    }
}
