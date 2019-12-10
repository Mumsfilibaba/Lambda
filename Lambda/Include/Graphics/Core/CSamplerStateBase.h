#pragma once
#include "CDeviceObjectBase.h"
#include "ISamplerState.h"

namespace Lambda
{
	//-----------------
	//CSamplerStateBase
	//-----------------

	template<typename TDeviceImpl>
	class CSamplerStateBase : public CDeviceObjectBase<TDeviceImpl, ISamplerState>
	{
	public:
        CSamplerStateBase(TDeviceImpl* pDevice, const SSamplerStateDesc& desc)
            : CDeviceObjectBase<TDeviceImpl, ISamplerState>(pDevice),
            m_Desc(desc)
        {
        }
        ~CSamplerStateBase() = default;

        LAMBDA_NO_COPY(CSamplerStateBase);

		virtual void* GetNativeHandle() const override = 0;
		virtual const SSamplerStateDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		SSamplerStateDesc m_Desc;
	};
}
