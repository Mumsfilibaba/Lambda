#pragma once
#include "CDeviceObjectBase.h"
#include "IPipelineState.h"

namespace Lambda
{
	//------------------
	//CPipelineStateBase
	//------------------

	template<typename TDeviceImpl>
	class CPipelineStateBase : public CDeviceObjectBase<TDeviceImpl, IPipelineState>
	{
	public:
        CPipelineStateBase(TDeviceImpl* pDevice, const SPipelineStateDesc& desc)
            : CDeviceObjectBase<TDeviceImpl, IPipelineState>(pDevice),
            m_Desc(desc)
        {
        }
        ~CPipelineStateBase() = default;

        LAMBDA_NO_COPY(CPipelineStateBase);

		virtual void CreateShaderVariableTable(IShaderVariableTable** ppVariableTable) override = 0;
		virtual void* GetNativeHandle() const override = 0;

		virtual const SPipelineStateDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		SPipelineStateDesc m_Desc;
	};
}
