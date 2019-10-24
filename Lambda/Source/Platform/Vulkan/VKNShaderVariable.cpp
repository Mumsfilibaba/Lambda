#include "LambdaPch.h"
#include "VKNShaderVariable.h"
#include "VKNShaderVariableTable.h"
#include "VKNTexture.h"
#include "VKNSamplerState.h"
#include "VKNDeviceContext.h"

namespace Lambda
{
	//-----------------
	//VKNShaderVariable
	//-----------------

	VKNShaderVariable::VKNShaderVariable(VKNDevice* pDevice, VKNShaderVariableTable* pVariableTable, const ShaderVariableDesc& desc)
		: ShaderVariableBase<VKNDevice>(pDevice),
		m_pVariableTable(nullptr),
		m_Resource(nullptr),
        m_ResourceHandle(VK_NULL_HANDLE)
	{
		this->AddRef();

		LAMBDA_ASSERT(pVariableTable != nullptr);

		//Since the ShaderVariableTable owns the memory to the 
		//shadervariable we only need a weak pointer to shadervariable
		m_pVariableTable = pVariableTable;
		m_Desc = desc;
        
        //Setup descriptor write
        m_DescriptorWrite.sType             = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        m_DescriptorWrite.pNext             = nullptr;
        m_DescriptorWrite.descriptorCount   = 1;
        m_DescriptorWrite.dstArrayElement   = 0;
        m_DescriptorWrite.dstBinding        = m_Desc.Slot;
        m_DescriptorWrite.pTexelBufferView  = nullptr;
        m_DescriptorWrite.descriptorType    = ConvertResourceToDescriptorType(m_Desc.Type, m_Desc.Usage);
        if (m_DescriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
        {
            //Init imageinfo
            m_ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            m_ImageInfo.imageView   = VK_NULL_HANDLE;
            //Get static sampler
            if (m_Desc.pSamplerState != nullptr)
            {
                m_DescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                m_ImageInfo.sampler = reinterpret_cast<VKNSamplerState*>(m_Desc.pSamplerState)->GetVkSampler();
            }
            else
            {
                m_ImageInfo.sampler = VK_NULL_HANDLE;
            }
            
            m_DescriptorWrite.pImageInfo = &m_ImageInfo;
        }
        else if (m_DescriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER || m_DescriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
        {
            //Init bufferinfo
            m_BufferInfo.buffer = VK_NULL_HANDLE;
            m_BufferInfo.offset = 0;
            m_BufferInfo.range  = 0;
            m_DescriptorWrite.pBufferInfo = &m_BufferInfo;
        }
        else if (m_DescriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER)
        {
            //Init imageinfo
            m_ImageInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            m_ImageInfo.imageView   = VK_NULL_HANDLE;
            m_ImageInfo.sampler     = VK_NULL_HANDLE;
            m_DescriptorWrite.pImageInfo = &m_ImageInfo;
        }
    }


	void VKNShaderVariable::SetTexture(ITexture* pTexture)
	{
		LAMBDA_ASSERT_PRINT(m_Desc.Type == RESOURCE_TYPE_TEXTURE, "Vulkan: Variable is not a Texture\n");
		LAMBDA_ASSERT_PRINT(pTexture != nullptr, "Vulkan: pTexture cannot be nullptr\n");
		
		if (m_Resource.Get() != pTexture)
		{
			//Make sure we have a reference to the resource
			pTexture->AddRef();
			m_Resource = pTexture;
		}
	}


	void VKNShaderVariable::SetConstantBuffer(IBuffer* pBuffer)
	{
		LAMBDA_ASSERT_PRINT(m_Desc.Type == RESOURCE_TYPE_CONSTANT_BUFFER, "Vulkan: Variable is not a ConstantBuffer\n");
		LAMBDA_ASSERT_PRINT(pBuffer != nullptr, "Vulkan: pBuffer cannot be nullptr\n");

		if (m_Resource.Get() != pBuffer)
		{
			//Make sure we have a reference to the resource
			pBuffer->AddRef();
			m_Resource = pBuffer;
		}
	}


	void VKNShaderVariable::SetSamplerState(ISamplerState* pSamplerState)
	{
		LAMBDA_ASSERT_PRINT(m_Desc.Type == RESOURCE_TYPE_SAMPLER_STATE, "Vulkan: Variable is not a SamplerState\n");
		LAMBDA_ASSERT_PRINT(pSamplerState != nullptr, "Vulkan: pSamplerState cannot be nullptr\n");

		if (m_Resource.Get() != pSamplerState)
		{
			//Make sure we have a reference to the resource
			pSamplerState->AddRef();
			m_Resource = pSamplerState;
		}
	}


	IShaderVariableTable* VKNShaderVariable::GetShaderVariableTable() const
	{
		m_pVariableTable->AddRef();
		return m_pVariableTable;
	}

	
	bool VKNShaderVariable::Validate()
	{
        //Here we get the handle to the resource that is bound,
        uint64 currentHandle = VK_NULL_HANDLE;
        if (m_Desc.Type == RESOURCE_TYPE_TEXTURE)
        {
            VKNTexture* pVkTexture	= m_Resource.GetAs<VKNTexture>();
            m_ImageInfo.imageView   = pVkTexture->GetVkImageView();
            m_ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            currentHandle = uint64(m_ImageInfo.imageView);
        }
        else if (m_Desc.Type == RESOURCE_TYPE_CONSTANT_BUFFER)
        {
            VKNBuffer* pVkBuffer = m_Resource.GetAs<VKNBuffer>();
            m_BufferInfo.buffer  = pVkBuffer->GetVkBuffer();
            m_BufferInfo.offset  = 0;
        
            const BufferDesc& desc = pVkBuffer->GetDesc();
            m_BufferInfo.range = desc.SizeInBytes;
            
            currentHandle = uint64(m_BufferInfo.buffer);
        }
        else if (m_Desc.Type == RESOURCE_TYPE_SAMPLER_STATE)
        {
            VKNSamplerState* pVkSamplerState = m_Resource.GetAs<VKNSamplerState>();
            m_ImageInfo.imageView   = VK_NULL_HANDLE;
            m_ImageInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            m_ImageInfo.sampler     = pVkSamplerState->GetVkSampler();
            
            currentHandle = uint64(m_ImageInfo.sampler);
        }
        
        //We check if the handle that is bound is the same as the one we have written to the descriptorset.
        //In that case the validation passes and we do not need to write this descriptor again
        uint64 oldHandle = m_ResourceHandle;
        m_ResourceHandle = currentHandle;
        return oldHandle == currentHandle;
	}


	void VKNShaderVariable::Transition(VKNDeviceContext* pContext)
	{
		if (m_Desc.Type == RESOURCE_TYPE_TEXTURE)
		{
			VKNTexture* pVkTexture = m_Resource.GetAs<VKNTexture>();
			pContext->TransitionTexture(pVkTexture, RESOURCE_STATE_PIXEL_SHADER_RESOURCE, VK_REMAINING_MIP_LEVELS);
		}
	}
}
