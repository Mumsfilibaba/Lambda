#include "LambdaPch.h"
#include "VKNShaderVariable.h"
#include "VKNShaderVariableTable.h"
#include "VKNBuffer.h"
#include "VKNTexture.h"
#include "VKNSamplerState.h"

namespace Lambda
{
	//-----------------
	//VKNShaderVariable
	//-----------------

	VKNShaderVariable::VKNShaderVariable(VKNDevice* pDevice, VKNShaderVariableTable* pVariableTable, const ShaderVariableDesc& desc)
		: DeviceObjectBase<VKNDevice, IShaderVariable>(pDevice),
		m_pVariableTable(nullptr),
		m_Resource(nullptr),
		m_Desc()
	{
		this->AddRef();

		LAMBDA_ASSERT(pVariableTable != nullptr);

		//Since the ShaderVariableTable owns the memory to the 
		//shadervariable we only need a weak pointer to shadervariable
		m_pVariableTable = pVariableTable;
		m_Desc = desc;

		//Get static sampler
		if (m_Desc.pSamplerState == nullptr)
			m_ImageInfo.sampler = VK_NULL_HANDLE;
		else
			m_ImageInfo.sampler = reinterpret_cast<VKNSamplerState*>(m_Desc.pSamplerState)->GetVkSampler();
	}


	void VKNShaderVariable::SetTexture(ITexture* pTexture)
	{
		LAMBDA_ASSERT_PRINT(m_Desc.Type == RESOURCE_TYPE_TEXTURE, "Vulkan: Variable is not a Texture\n");
		LAMBDA_ASSERT_PRINT(pTexture != nullptr, "Vulkan: pTexture cannot be nullptr\n");
		
		if (m_Resource.Get() != pTexture)
		{
			//Setup descriptor write
			VKNTexture* pVkTexture  = reinterpret_cast<VKNTexture*>(pTexture);
			m_ImageInfo.imageView	= pVkTexture->GetVkImageView();
			m_ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			//Make sure we have a reference to the resource
			pTexture->AddRef();
			m_Resource = pTexture;

			//Make sure the table gets updated
			m_pVariableTable->Invalidate();
		}
	}


	void VKNShaderVariable::SetConstantBuffer(IBuffer* pBuffer)
	{
		LAMBDA_ASSERT_PRINT(m_Desc.Type == RESOURCE_TYPE_CONSTANT_BUFFER, "Vulkan: Variable is not a ConstantBuffer\n");
		LAMBDA_ASSERT_PRINT(pBuffer != nullptr, "Vulkan: pBuffer cannot be nullptr\n");

		if (m_Resource.Get() != pBuffer)
		{
			//Setup descriptor write
			VKNBuffer* pVkBuffer = reinterpret_cast<VKNBuffer*>(pBuffer);
			m_BufferInfo.buffer  = pVkBuffer->GetVkBuffer();
			m_BufferInfo.offset  = pVkBuffer->GetDynamicOffset();
		
			const BufferDesc& desc = pVkBuffer->GetDesc();
			m_BufferInfo.range = desc.SizeInBytes;

			//Make sure we have a reference to the resource
			pBuffer->AddRef();
			m_Resource = pBuffer;

			//Make sure the table gets updated
			m_pVariableTable->Invalidate();
		}
	}


	void VKNShaderVariable::SetSamplerState(ISamplerState* pSamplerState)
	{
		LAMBDA_ASSERT_PRINT(m_Desc.Type == RESOURCE_TYPE_SAMPLER_STATE, "Vulkan: Variable is not a SamplerState\n");
		LAMBDA_ASSERT_PRINT(pSamplerState != nullptr, "Vulkan: pSamplerState cannot be nullptr\n");

		if (m_Resource.Get() != pSamplerState)
		{
			//Setup descriptor write
			VKNSamplerState* pVkSamplerState = reinterpret_cast<VKNSamplerState*>(pSamplerState);
			m_ImageInfo.imageView	= VK_NULL_HANDLE;
			m_ImageInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			m_ImageInfo.sampler		= pVkSamplerState->GetVkSampler();

			//Make sure we have a reference to the resource
			pSamplerState->AddRef();
			m_Resource = pSamplerState;

			//Make sure the table gets updated
			m_pVariableTable->Invalidate();
		}
	}


	IShaderVariableTable* VKNShaderVariable::GetShaderVariableTable() const
	{
		m_pVariableTable->AddRef();
		return m_pVariableTable;
	}


	const ShaderVariableDesc& VKNShaderVariable::GetDesc() const
	{
		return m_Desc;
	}

	
	void VKNShaderVariable::Verify()
	{
	}
	
	
	uint32 VKNShaderVariable::GetDynamicOffset() const
	{
		if (m_Desc.Type == RESOURCE_TYPE_CONSTANT_BUFFER && m_Desc.Usage == RESOURCE_USAGE_DYNAMIC)
			return reinterpret_cast<VKNBuffer*>(m_Resource.Get())->GetDynamicOffset();

		return 0;
	}
}