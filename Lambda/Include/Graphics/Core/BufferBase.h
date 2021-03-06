#pragma once
#include "DeviceObjectBase.h"
#include "IBuffer.h"

namespace Lambda
{
	//----------
	//BufferBase
	//----------

	template<typename TDeviceImpl>
	class BufferBase : public DeviceObjectBase<TDeviceImpl, IBuffer>
	{
	public:
		LAMBDA_NO_COPY(BufferBase);

		BufferBase(TDeviceImpl* pDevice, const BufferDesc& desc)
			: DeviceObjectBase<TDeviceImpl, IBuffer>(pDevice),
			m_Desc(desc)
		{
		}
		~BufferBase() = default;

		virtual void* GetNativeHandle() const override = 0;

		virtual const BufferDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		BufferDesc m_Desc;
	};
}