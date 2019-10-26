#include "LambdaPch.h"
#include "VKNDevice.h"
#include "VKNShader.h"

namespace Lambda
{
	//---------
	//VKNShader
	//---------

	VKNShader::VKNShader(VKNDevice* pDevice, const ShaderDesc& desc)
		: ShaderBase<VKNDevice>(pDevice, desc),
		m_Shader(VK_NULL_HANDLE),
		m_ByteCode(),
		m_EntryPoint()
	{
		//Add a ref to the refcounter
		this->AddRef();
        Init(desc);
    }


	VKNShader::~VKNShader()
	{
		if (m_Shader != VK_NULL_HANDLE)
			m_pDevice->SafeReleaseVulkanResource<VkShaderModule>(m_Shader);

		LOG_DEBUG_INFO("Vulkan: Destroyed Shader\n");
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
        if (vkCreateShaderModule(m_pDevice->GetVkDevice(), &info, nullptr, &m_Shader) != VK_SUCCESS)
        {
            LOG_DEBUG_ERROR("Vulkan: Failed to create shadermodule\n");
            m_Shader = VK_NULL_HANDLE;
        }
        else
        {
            LOG_DEBUG_INFO("Vulkan: Created shader\n");

            m_EntryPoint = std::string(desc.pEntryPoint);
			m_Desc.pSource = nullptr;
			m_Desc.pEntryPoint = m_EntryPoint.c_str();
        }
    }

    
    void* VKNShader::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_Shader);
    }
}
