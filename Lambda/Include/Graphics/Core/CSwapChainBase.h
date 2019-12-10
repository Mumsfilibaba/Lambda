#pragma once
#include "CDeviceObjectBase.h"
#include "ISwapChain.h"

namespace Lambda
{
	//--------------
	//CSwapChainBase
	//--------------

	template<typename TDeviceImpl>
	class CSwapChainBase : public CDeviceObjectBase<TDeviceImpl, ISwapChain>
	{
	public:
        CSwapChainBase(TDeviceImpl* pDevice, const SSwapChainDesc& desc)
            : CDeviceObjectBase<TDeviceImpl, ISwapChain>(pDevice),
            m_Desc(desc)
        {
        }
        ~CSwapChainBase() = default;

        LAMBDA_NO_COPY(CSwapChainBase);

		virtual void ResizeBuffers(uint32 width, uint32 height) override = 0;
		virtual void Present() override = 0;
		virtual bool SetFullscreenState(bool fullscreenState) override = 0;
		virtual bool GetFullscreenState() const override = 0;
		virtual ITexture* GetBuffer() override = 0;
		virtual ITexture* GetDepthBuffer() override = 0;
		virtual void* GetNativeHandle() const override = 0;


		virtual const SSwapChainDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		SSwapChainDesc m_Desc;
	};
}
