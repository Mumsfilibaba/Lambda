#pragma once
#include "Graphics/Core/ShaderVariableBase.h"
#include "VKNBuffer.h"

namespace Lambda
{
	class VKNDevice;
	class VKNDeviceContext;
	class VKNShaderVariableTable;

	//----------------------
	//VKNShaderVariableTable
	//----------------------

	class VKNShaderVariable : public ShaderVariableBase<VKNDevice, VKNShaderVariableTable>
	{
		using TShaderVariable = ShaderVariableBase<VKNDevice, VKNShaderVariableTable>;

	public:
		LAMBDA_NO_COPY(VKNShaderVariable);

		VKNShaderVariable(VKNDevice* pDevice, VKNShaderVariableTable* pVariableTable, const ShaderVariableDesc& desc);
		~VKNShaderVariable() = default;

		virtual void SetTexture(ITexture* pTexture) override final;
		virtual void SetConstantBuffer(IBuffer* pBuffer) override final;
		virtual void SetSamplerState(ISamplerState* pSamplerState) override final;

		bool Validate();


		_forceinline uint32 GetDynamicOffset() const
		{
			if (m_Desc.Type == RESOURCE_TYPE_CONSTANT_BUFFER && m_Desc.Usage == USAGE_DYNAMIC)
				return uint32(reinterpret_cast<VKNBuffer*>(m_Resource.Get())->GetDynamicOffset());

			return 0;
		}


		_forceinline const VkWriteDescriptorSet& GetVkWriteDescriptorSet()
		{ 
			return m_DescriptorWrite; 
		};
	private:
		AutoRef<IDeviceObject>	m_Resource;
        VkWriteDescriptorSet    m_DescriptorWrite;
        uint64                  m_ResourceHandle;
		union
		{
			VkDescriptorBufferInfo	m_BufferInfo;
			VkDescriptorImageInfo	m_ImageInfo;
		};
		bool m_IsValid;
	};
}
