#include "LambdaPch.h"
#include "VKNShaderVariable.h"
#include "VKNShaderVariableTable.h"
#include "VKNTexture.h"
#include "VKNSamplerState.h"
#include "VKNDevice.h"

namespace Lambda
{
	//-----------------
	//VKNShaderVariable
	//-----------------

	VKNShaderVariable::VKNShaderVariable(VKNDevice* pDevice, VKNShaderVariableTable* pShaderVariableTable, const ShaderVariableDesc& desc)
		: TShaderVariable(pDevice, pShaderVariableTable, desc),
		m_Resource(nullptr),
        m_ResourceHandle(VK_NULL_HANDLE),
		m_IsValid(false)
	{
		//Increase inital ref
		this->AddRef();
		        
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
			VKNTexture* pVkDefaultTexture = m_pDevice->GetDefaultTexture();
			pVkDefaultTexture->AddRef();
			m_Resource = pVkDefaultTexture;

            m_ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            m_ImageInfo.imageView   = pVkDefaultTexture->GetVkImageView();
            
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
			VKNBuffer* pVkDefaultConstantBuffer = m_pDevice->GetDefaultConstantBuffer();
			pVkDefaultConstantBuffer->AddRef();
			m_Resource = pVkDefaultConstantBuffer;

			m_BufferInfo.buffer = pVkDefaultConstantBuffer->GetVkBuffer();
            m_BufferInfo.offset = 0;
            m_BufferInfo.range  = pVkDefaultConstantBuffer->GetDesc().SizeInBytes;
            m_DescriptorWrite.pBufferInfo = &m_BufferInfo;
        }
        else if (m_DescriptorWrite.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER)
        {
            //Init imageinfo
			VKNSamplerState* pVkDefaultSamplerState = m_pDevice->GetDefaultSamplerState();
			pVkDefaultSamplerState->AddRef();
			m_Resource = pVkDefaultSamplerState;

            m_ImageInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            m_ImageInfo.imageView   = VK_NULL_HANDLE;
            m_ImageInfo.sampler     = pVkDefaultSamplerState->GetVkSampler();
            m_DescriptorWrite.pImageInfo = &m_ImageInfo;
        }
    }


	void VKNShaderVariable::SetTexture(ITexture* pTexture)
	{
		LAMBDA_ASSERT_PRINT(m_Desc.Type == RESOURCE_TYPE_TEXTURE, "Vulkan: Variable is not a Texture\n");
		
		//If texture is nullptr we get the default texture
		VKNTexture* pVkTexture = reinterpret_cast<VKNTexture*>(pTexture);
		if (pVkTexture == nullptr)
		{
			pVkTexture = m_pDevice->GetDefaultTexture();
		}
		else
		{
			LAMBDA_ASSERT_PRINT(pVkTexture->GetDesc().Flags & TEXTURE_FLAGS_SHADER_RESOURCE, "Vulkan: pTexture does not have the flag TEXTURE_FLAGS_SHADER_RESOURCE\n");
		}


		//Make sure that this resource is not already the one bound to the table
		if (m_Resource.Get() != pVkTexture)
		{
			//Make sure we have a reference to the resource
			pVkTexture->AddRef();
			m_Resource = pVkTexture;

			//Invalidate variable
			m_IsValid = false;
		}
	}


	void VKNShaderVariable::SetConstantBuffer(IBuffer* pBuffer)
	{
		LAMBDA_ASSERT_PRINT(m_Desc.Type == RESOURCE_TYPE_CONSTANT_BUFFER, "Vulkan: Variable is not a ConstantBuffer\n");
		
		//If buffer is nullptr we get the default constantbuffer
		VKNBuffer* pVkBuffer = reinterpret_cast<VKNBuffer*>(pBuffer);
		if (pVkBuffer == nullptr)
		{
			pVkBuffer = m_pDevice->GetDefaultConstantBuffer();
		}
		else
		{
			LAMBDA_ASSERT_PRINT(pVkBuffer->GetDesc().Flags & BUFFER_FLAGS_CONSTANT_BUFFER, "Vulkan: pBuffer does not have the flag BUFFER_FLAGS_CONSTANT_BUFFER\n");
		}

		
		//Make sure we are not binding the constantbuffer already used
		if (m_Resource.Get() != pVkBuffer)
		{
			//Make sure we have a reference to the resource
			pVkBuffer->AddRef();
			m_Resource = pVkBuffer;

			//Invalidate variable
			m_IsValid = false;
		}
	}


	void VKNShaderVariable::SetSamplerState(ISamplerState* pSamplerState)
	{
		LAMBDA_ASSERT_PRINT(m_Desc.Type == RESOURCE_TYPE_SAMPLER_STATE, "Vulkan: Variable is not a SamplerState\n");

		//If samplerstate is nullptr we get the default samplerstate
		VKNSamplerState* pVkSamplerState = reinterpret_cast<VKNSamplerState*>(pSamplerState);
		if (pVkSamplerState == nullptr)
			pVkSamplerState = m_pDevice->GetDefaultSamplerState();


		//Make sure that we are not binding the samplerstate we already have bound
		if (m_Resource.Get() != pVkSamplerState)
		{
			//Make sure we have a reference to the resource
			pVkSamplerState->AddRef();
			m_Resource = pVkSamplerState;

			//Invalidate variable
			m_IsValid = false;
		}
	}

	
	bool VKNShaderVariable::Validate()
	{
		//If this is not a dynamic variable we just return if we have bound a new resource
		if (m_Desc.Usage != USAGE_DYNAMIC && m_IsValid)
		{
			return true;
		}

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

		m_IsValid = true;
        return oldHandle == currentHandle;
	}
}
