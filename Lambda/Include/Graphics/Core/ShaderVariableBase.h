#pragma once
#include "DeviceObjectBase.h"
#include "IShaderVariableTable.h"

namespace Lambda
{
	//------------------
	//ShaderVariableBase
	//------------------

	template<typename TDeviceImpl, typename TShaderVariableTableImpl>
	class ShaderVariableBase : public DeviceObjectBase<TDeviceImpl, IShaderVariable>
	{
	public:
		LAMBDA_NO_COPY(ShaderVariableBase);

		ShaderVariableBase(TDeviceImpl* pDevice, TShaderVariableTableImpl* pShaderVariableTable, const ShaderVariableDesc& desc)
			: DeviceObjectBase<TDeviceImpl, IShaderVariable>(pDevice),
			m_Desc(desc),
			m_pShaderVariableTable(pShaderVariableTable)
		{
			LAMBDA_ASSERT(pShaderVariableTable != nullptr);
		}
		~ShaderVariableBase() = default;

		virtual void SetTexture(ITexture* pTexture) override = 0;
		virtual void SetConstantBuffer(IBuffer* pBuffer) override = 0;
		virtual void SetSamplerState(ISamplerState* pSamplerState) override = 0;


		virtual IShaderVariableTable* GetShaderVariableTable() const override
		{
			m_pShaderVariableTable->AddRef();
			return m_pShaderVariableTable;
		}


		virtual const ShaderVariableDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		//Since the ShaderVariableTable owns the memory to the 
		//shadervariable we only need a weak pointer to shadervariable
		TShaderVariableTableImpl* m_pShaderVariableTable;
		ShaderVariableDesc m_Desc;
	};
}