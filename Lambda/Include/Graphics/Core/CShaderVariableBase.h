#pragma once
#include "CDeviceObjectBase.h"
#include "IShaderVariableTable.h"

namespace Lambda
{
	//-------------------
	//CShaderVariableBase
	//-------------------

	template<typename TDeviceImpl, typename TShaderVariableTableImpl>
	class CShaderVariableBase : public CDeviceObjectBase<TDeviceImpl, IShaderVariable>
	{
	public:
        CShaderVariableBase(TDeviceImpl* pDevice, TShaderVariableTableImpl* pShaderVariableTable, const SShaderVariableDesc& desc)
            : CDeviceObjectBase<TDeviceImpl, IShaderVariable>(pDevice),
            m_Desc(desc),
            m_pShaderVariableTable(pShaderVariableTable)
        {
            LAMBDA_ASSERT(pShaderVariableTable != nullptr);
        }
        ~CShaderVariableBase() = default;

        LAMBDA_NO_COPY(CShaderVariableBase);

		virtual void SetTexture(ITexture* pTexture) override = 0;
		virtual void SetConstantBuffer(IBuffer* pBuffer) override = 0;
		virtual void SetSamplerState(ISamplerState* pSamplerState) override = 0;


		virtual IShaderVariableTable* GetShaderVariableTable() const override
		{
			m_pShaderVariableTable->AddRef();
			return m_pShaderVariableTable;
		}


		virtual const SShaderVariableDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		//Since the ShaderVariableTable owns the memory to the 
		//shadervariable we only need a weak pointer to shadervariable
		TShaderVariableTableImpl* m_pShaderVariableTable;
		SShaderVariableDesc m_Desc;
	};
}
