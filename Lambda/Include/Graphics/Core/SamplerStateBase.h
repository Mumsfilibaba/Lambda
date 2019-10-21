#pragma once
#include "DeviceObjectBase.h"
#include "ISamplerState.h"

namespace Lambda
{
	//----------------
	//SamplerStateBase
	//----------------

	template<typename TDeviceImpl>
	class SamplerStateBase : public DeviceObjectBase<TDeviceImpl, ISamplerState>
	{
	public:
		LAMBDA_NO_COPY(SamplerStateBase);

		SamplerStateBase(TDeviceImpl* pDevice)
			: DeviceObjectBase<TDeviceImpl, ISamplerState>(pDevice),
			m_Desc()
		{
		}
		~SamplerStateBase() = default;


		virtual const SamplerStateDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		SamplerStateDesc m_Desc;
	};
}