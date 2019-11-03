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

		SamplerStateBase(TDeviceImpl* pDevice, const SamplerStateDesc& desc)
			: DeviceObjectBase<TDeviceImpl, ISamplerState>(pDevice),
			m_Desc(desc)
		{
		}
		~SamplerStateBase() = default;

		virtual void* GetNativeHandle() const override = 0;


		virtual const SamplerStateDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		SamplerStateDesc m_Desc;
	};
}