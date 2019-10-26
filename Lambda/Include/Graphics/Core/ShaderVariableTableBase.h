#pragma once
#include "DeviceObjectBase.h"
#include "IShaderVariableTable.h"

namespace Lambda
{
	//-----------------------
	//ShaderVariableTableBase
	//-----------------------

	template<typename TDeviceImpl, typename TPipelineStateImpl>
	class ShaderVariableTableBase : public DeviceObjectBase<TDeviceImpl, IShaderVariableTable>
	{
	public:
		LAMBDA_NO_COPY(ShaderVariableTableBase);

		ShaderVariableTableBase(TDeviceImpl* pDevice, TPipelineStateImpl* pPipelineState)
			: DeviceObjectBase<TDeviceImpl, IShaderVariableTable>(pDevice),
			m_PipelineState(pPipelineState)
		{
			LAMBDA_ASSERT(pPipelineState != nullptr);
			
			//Add ref to parent
			m_PipelineState->AddRef();
		}
		~ShaderVariableTableBase() = default;

		virtual IShaderVariable* GetVariableByName(ShaderStage shader, const char* pName) override = 0;
		virtual IShaderVariable* GetVariableByIndex(ShaderStage shader, uint32 index) override = 0;
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