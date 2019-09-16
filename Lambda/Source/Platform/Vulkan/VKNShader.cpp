#include "LambdaPch.h"
#include "VKNDevice.h"
#include "VKNShader.h"

namespace Lambda
{
	//---------
	//VKNShader
	//---------

	VKNShader::VKNShader(const ShaderDesc& desc)
		: m_Shader(VK_NULL_HANDLE),
		m_ByteCode(),
		m_EntryPoint(),
		m_Desc()
	{
        Init(desc);
    }
    
    
    void VKNShader::Init(const ShaderDesc& desc)
    {
        //Get the bytecode
        if (desc.Languange == SHADER_LANG_SPIRV)
        {
            m_ByteCode = std::vector<char>(desc.pSource, desc.pSource + desc.SourceLength);
        }
        else
        {
            LOG_DEBUG_ERROR("Vulkan: Language not supported\n");
        }
        
		//Create a vulkan shader
        VkShaderModuleCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.codeSize = desc.SourceLength;
        info.pCode = reinterpret_cast<const uint32_t*>(m_ByteCode.data());
        
		VKNDevice& device = VKNDevice::GetInstance();
        if (vkCreateShaderModule(device.GetDevice(), &info, nullptr, &m_Shader) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create shadermodule\n");
            m_Shader = VK_NULL_HANDLE;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created shader\n");

            m_EntryPoint = std::string(desc.pEntryPoint);
			m_Desc = desc;
			m_Desc.pSource = nullptr;
			m_Desc.pEntryPoint = m_EntryPoint.c_str();
        }
    }
    
    
    void VKNShader::Destroy(VkDevice device)
    {
		LAMBDA_ASSERT(device != VK_NULL_HANDLE);

        if (m_Shader != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(device, m_Shader, nullptr);
            m_Shader = VK_NULL_HANDLE;
        }
        
        delete this;
    }
    
    
    void* VKNShader::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_Shader);
    }
	
	
	ShaderDesc VKNShader::GetDesc() const
	{
		return m_Desc;
	}
}
