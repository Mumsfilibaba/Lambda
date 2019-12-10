#pragma once
#include "CDeviceObjectBase.h"
#include "IBuffer.h"

namespace Lambda
{
	//-----------
	//CBufferBase
	//-----------

	template<typename TDeviceImpl>
	class CBufferBase : public DeviceObjectBase<TDeviceImpl, IBuffer>
	{
	public:
        CBufferBase(TDeviceImpl* pDevice, const BufferDesc& desc)
            : DeviceObjectBase<TDeviceImpl, IBuffer>(pDevice),
            m_Desc(desc)
        {
        }
        ~CBufferBase() = default;

        LAMBDA_NO_COPY(CBufferBase);

		virtual void* GetNativeHandle() const override = 0;
		virtual const BufferDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		BufferDesc m_Desc;
	};
}
