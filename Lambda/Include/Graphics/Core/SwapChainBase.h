#pragma once
#include "DeviceObjectBase.h"
#include "ISwapChain.h"

namespace Lambda
{
	//-------------
	//SwapChainBase
	//-------------

	template<typename TDeviceImpl>
	class SwapChainBase : public DeviceObjectBase<TDeviceImpl, ISwapChain>
	{
	public:
		LAMBDA_NO_COPY(SwapChainBase);

		SwapChainBase(TDeviceImpl* pDevice)
			: DeviceObjectBase<TDeviceImpl, ISwapChain>(pDevice),
			m_Desc()
		{
		}
		~SwapChainBase() = default;


		virtual const SwapChainDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		SwapChainDesc m_Desc;
	};
}