#pragma once
#include "CDeviceObjectBase.h"
#include "IShaderVariableTable.h"
#include "Core/AutoRef.h"

namespace Lambda
{
	//------------------------
	//CShaderVariableTableBase
	//------------------------

	template<typename TDeviceImpl, typename TPipelineStateImpl>
	class CShaderVariableTableBase : public CDeviceObjectBase<TDeviceImpl, IShaderVariableTable>
	{
	public:
        CShaderVariableTableBase(TDeviceImpl* pDevice, TPipelineStateImpl* pPipelineState)
            : CDeviceObjectBase<TDeviceImpl, IShaderVariableTable>(pDevice),
            m_PipelineState(pPipelineState)
        {
            LAMBDA_ASSERT(pPipelineState != nullptr);
            
            //Add ref to parent
            m_PipelineState->AddRef();
        }
        ~CShaderVariableTableBase() = default;

        LAMBDA_NO_COPY(CShaderVariableTableBase);

		virtual IShaderVariable* GetVariableByName(EShaderStage shader, const char* pName) override = 0;
		virtual IShaderVariable* GetVariableByIndex(EShaderStage shader, uint32 index) override = 0;
		virtual uint32 GetVariableCount() const override = 0;

		virtual IPipelineState* GetPipelineState() const override
		{
			//Add ref before returning it
			m_PipelineState->AddRef();
			return m_PipelineState.Get();
		}
	protected:
		AutoRef<TPipelineStateImpl> m_PipelineState;
	};
}
