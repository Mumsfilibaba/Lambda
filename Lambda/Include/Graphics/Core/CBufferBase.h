#pragma once
#include "CDeviceObjectBase.h"
#include "IBuffer.h"

namespace Lambda
{
	//-----------
	//CBufferBase
	//-----------

	template<typename TDeviceImpl>
	class CBufferBase : public CDeviceObjectBase<TDeviceImpl, IBuffer>
	{
	public:
        CBufferBase(TDeviceImpl* pDevice, const SBufferDesc& desc)
            : CDeviceObjectBase<TDeviceImpl, IBuffer>(pDevice),
            m_Desc(desc)
        {
        }
        ~CBufferBase() = default;

        LAMBDA_NO_COPY(CBufferBase);

		virtual void* GetNativeHandle() const override = 0;
		virtual const SBufferDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		SBufferDesc m_Desc;
	};
}
