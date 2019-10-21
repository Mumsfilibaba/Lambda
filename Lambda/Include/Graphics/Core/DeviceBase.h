#pragma once
#include "DeviceObjectBase.h"
#include "IDevice.h"

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

		BufferBase(TDeviceImpl* pDevice)
			: DeviceObjectBase<TDeviceImpl, IBuffer>(pDevice),
			m_Desc()
		{
		}
		~BufferBase() = default;


		virtual const BufferDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		BufferDesc m_Desc;
	};
}