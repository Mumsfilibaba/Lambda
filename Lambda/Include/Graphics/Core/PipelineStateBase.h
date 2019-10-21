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

		PipelineStateBase(TDeviceImpl* pDevice)
			: DeviceObjectBase<TDeviceImpl, IPipelineState>(pDevice),
			m_Desc()
		{
		}
		~PipelineStateBase() = default;


		virtual const PipelineStateDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		PipelineStateDesc m_Desc;
	};
}