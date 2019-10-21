#pragma once
#include "Graphics/Core/ShaderVariableBase.h"
#include <vulkan/vulkan.h>

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
		void Transition(VKNDeviceContext* pContext);
		bool Validate();

		uint32 GetDynamicOffset() const;
		inline const VkWriteDescriptorSet& GetVkWriteDescriptorSet() { return m_DescriptorWrite; };
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
