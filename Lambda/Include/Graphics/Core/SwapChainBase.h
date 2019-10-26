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

		SwapChainBase(TDeviceImpl* pDevice, const SwapChainDesc& desc)
			: DeviceObjectBase<TDeviceImpl, ISwapChain>(pDevice),
			m_Desc(desc)
		{
		}
		~SwapChainBase() = default;

		virtual void ResizeBuffers(uint32 width, uint32 height) override = 0;
		virtual void Present() override = 0;
		virtual bool SetFullscreenState(bool fullscreenState) override = 0;
		virtual bool GetFullscreenState() const override = 0;
		virtual ITexture* GetBuffer() override = 0;
		virtual ITexture* GetDepthBuffer() override = 0;
		virtual void* GetNativeHandle() const override = 0;


		virtual const SwapChainDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		SwapChainDesc m_Desc;
	};
}