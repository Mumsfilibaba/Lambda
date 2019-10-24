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

	class VKNShaderVariable : public ShaderVariableBase<VKNDevice>
	{
	public:
		LAMBDA_NO_COPY(VKNShaderVariable);

		VKNShaderVariable(VKNDevice* pDevice, VKNShaderVariableTable* pVariableTable, const ShaderVariableDesc& desc);
		~VKNShaderVariable() = default;

		virtual void SetTexture(ITexture* pTexture) override final;
		virtual void SetConstantBuffer(IBuffer* pBuffer) override final;
		virtual void SetSamplerState(ISamplerState* pSamplerState) override final;
		virtual IShaderVariableTable* GetShaderVariableTable() const override final;
		bool Validate();
		void Transition(VKNDeviceContext* pContext);


		_forceinline uint32 GetDynamicOffset() const
		{
			if (m_Desc.Type == RESOURCE_TYPE_CONSTANT_BUFFER && m_Desc.Usage == RESOURCE_USAGE_DYNAMIC)
				return uint32(reinterpret_cast<VKNBuffer*>(m_Resource.Get())->GetDynamicOffset());

			return 0;
		}


		inline const VkWriteDescriptorSet& GetVkWriteDescriptorSet() 
		{ 
			return m_DescriptorWrite; 
		};
	private:
		VKNShaderVariableTable* m_pVariableTable;
		AutoRef<IDeviceObject>	m_Resource;
        VkWriteDescriptorSet    m_DescriptorWrite;
        uint64                  m_ResourceHandle;
		union
		{
			VkDescriptorBufferInfo	m_BufferInfo;
			VkDescriptorImageInfo	m_ImageInfo;
		};
	};
}
