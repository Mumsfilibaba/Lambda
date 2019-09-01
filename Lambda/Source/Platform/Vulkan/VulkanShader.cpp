#include "LambdaPch.h"
#include "VulkanShader.h"

namespace Lambda
{
    VulkanShader::VulkanShader(VkDevice device, const ShaderDesc& desc)
        : m_Shader(VK_NULL_HANDLE),
        m_ByteCode(),
        m_Type(SHADER_STAGE_UNKNOWN)
    {
        Init(device, desc);
    }
    
    
    void VulkanShader::Init(VkDevice device, const ShaderDesc& desc)
    {
        //Get the bytecode
        if (desc.Languange == SHADER_LANG_SPIRV)
        {
            //Copy type
            m_Type = desc.Type;
            
            //Copy source to array
            m_ByteCode = std::vector<char>(desc.pSource, desc.pSource + desc.SourceLength);
        }
        else
        {
            LOG_DEBUG_ERROR("Vulkan: Language not supported\n");
        }
        
        //Setup a vulkan shader
        VkShaderModuleCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.codeSize = desc.SourceLength;
        info.pCode = reinterpret_cast<const uint32_t*>(m_ByteCode.data());
        
        //Create a vulkan shader
        if (vkCreateShaderModule(device, &info, nullptr, &m_Shader) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create shadermodule\n");
            
            //Set shader to null otherwise destroy may fail
            m_Shader = VK_NULL_HANDLE;

        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created shader\n");
            m_EntryPoint = std::string(desc.pEntryPoint);
        }
    }
    
    
    void VulkanShader::Destroy(VkDevice device)
    {
        if (m_Shader != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(device, m_Shader, nullptr);
            m_Shader = VK_NULL_HANDLE;
        }
        
        //Delete me
        delete this;
    }
    
    
    ShaderStage VulkanShader::GetType() const
    {
        return m_Type;
    }
    
    
    const char* VulkanShader::GetEntryPoint() const
    {
        return m_EntryPoint.c_str();
    }
    
    
    void* VulkanShader::GetNativeHandle() const
    {
        return (void*)m_Shader;
    }
}
