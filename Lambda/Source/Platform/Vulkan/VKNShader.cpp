#include "LambdaPch.h"
#include "VKNDevice.h"
#include "VKNShader.h"

namespace Lambda
{
	//---------
	//VKNShader
	//---------

	VKNShader::VKNShader(VKNDevice* pVkDevice, const ShaderDesc& desc)
		: ShaderBase<VKNDevice>(pVkDevice, desc),
		m_VkShaderModule(VK_NULL_HANDLE),
		m_ByteCode(),
		m_EntryPoint()
	{
		//Add a ref to the refcounter
		this->AddRef();
        Init(desc);
    }


	VKNShader::~VKNShader()
	{
		if (m_VkShaderModule != VK_NULL_HANDLE)
			m_pDevice->SafeReleaseVkResource<VkShaderModule>(m_VkShaderModule);

		LOG_RENDER_API_INFO("[Vulkan] Destroyed Shader\n");
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
			LOG_RENDER_API_ERROR("[Vulkan] Language not supported\n");
        }
        
		//Create a vulkan shader
        VkShaderModuleCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.codeSize = desc.SourceLength;
        info.pCode = reinterpret_cast<const uint32_t*>(m_ByteCode.data());
        if (vkCreateShaderModule(m_pDevice->GetVkDevice(), &info, nullptr, &m_VkShaderModule) != VK_SUCCESS)
        {
			LOG_RENDER_API_ERROR("[Vulkan] Failed to create shadermodule\n");
            m_VkShaderModule = VK_NULL_HANDLE;
        }
        else
        {
			LOG_RENDER_API_INFO("[Vulkan] Created shader\n");

            m_EntryPoint = std::string(desc.pEntryPoint);
			m_Desc.pSource = nullptr;
			m_Desc.pEntryPoint = m_EntryPoint.c_str();
        }
    }

    
    void* VKNShader::GetNativeHandle() const
    {
        return reinterpret_cast<void*>(m_VkShaderModule);
    }
}
