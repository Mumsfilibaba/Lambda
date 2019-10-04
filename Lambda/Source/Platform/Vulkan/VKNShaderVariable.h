#pragma once
#include "Graphics/Core/IShaderVariableTable.h"
#include "Graphics/Core/DeviceObjectBase.h"
#include <vulkan/vulkan.h>

namespace Lambda
{
	class VKNDevice;
	class VKNShaderVariableTable;

	//----------------------
	//VKNShaderVariableTable
	//----------------------

	class VKNShaderVariable : public DeviceObjectBase<VKNDevice, IShaderVariable>
	{
	public:
		LAMBDA_NO_COPY(VKNShaderVariable);

		VKNShaderVariable(VKNDevice* pDevice, VKNShaderVariableTable* pVariableTable, const ShaderVariableDesc& desc);
		~VKNShaderVariable() = default;

		virtual void SetTexture(ITexture* pTexture) override final;
		virtual void SetConstantBuffer(IBuffer* pBuffer) override final;
		virtual void SetSamplerState(ISamplerState* pSamplerState) override final;
		virtual IShaderVariableTable* GetShaderVariableTable() const override final;
		virtual const ShaderVariableDesc& GetDesc() const override final;
		bool Validate();

		uint32 GetDynamicOffset() const;
		inline const VkWriteDescriptorSet& GetVkWriteDescriptorSet() { return m_DescriptorWrite; };
	private:
		VKNShaderVariableTable* m_pVariableTable;
		AutoRef<IDeviceObject>	m_Resource;
		ShaderVariableDesc		m_Desc;
        VkWriteDescriptorSet    m_DescriptorWrite;
        uint64                  m_ResourceHandle;
		union
		{
			VkDescriptorBufferInfo	m_BufferInfo;
			VkDescriptorImageInfo	m_ImageInfo;
		};
	};
}
