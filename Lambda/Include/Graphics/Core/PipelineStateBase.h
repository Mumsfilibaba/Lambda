#pragma once
#include "DeviceObjectBase.h"
#include "IPipelineState.h"

namespace Lambda
{
	//-----------------
	//PipelineStateBase
	//-----------------

	template<typename TDeviceImpl>
	class PipelineStateBase : public DeviceObjectBase<TDeviceImpl, IPipelineState>
	{
	public:
		LAMBDA_NO_COPY(PipelineStateBase);

		PipelineStateBase(TDeviceImpl* pDevice, const PipelineStateDesc& desc)
			: DeviceObjectBase<TDeviceImpl, IPipelineState>(pDevice),
			m_Desc(desc)
		{
		}
		~PipelineStateBase() = default;

		virtual void CreateShaderVariableTable(IShaderVariableTable** ppVariableTable) override = 0;
		virtual void* GetNativeHandle() const override = 0;


		virtual const PipelineStateDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		PipelineStateDesc m_Desc;
	};
}