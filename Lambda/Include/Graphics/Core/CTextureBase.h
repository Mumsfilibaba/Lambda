#pragma once
#include "CDeviceObjectBase.h"
#include "ITexture.h"

namespace Lambda
{
	//------------
	//CTextureBase
	//------------

	template<typename TDeviceImpl>
	class CTextureBase : public CDeviceObjectBase<TDeviceImpl, ITexture>
	{
	public:

        CTextureBase(TDeviceImpl* pDevice, const STextureDesc& desc)
            : CDeviceObjectBase<TDeviceImpl, ITexture>(pDevice),
            m_Desc(desc)
        {
        }
        ~CTextureBase() = default;

        LAMBDA_NO_COPY(CTextureBase);

		virtual void* GetNativeHandle() const override = 0;
		virtual const STextureDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		STextureDesc m_Desc;
	};
}
