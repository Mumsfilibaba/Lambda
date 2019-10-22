#pragma once
#include "IDevice.h"

namespace Lambda
{
	//----------
	//DeviceBase
	//----------

	class DeviceBase : public RefCountedObject<IDevice>
	{
	public:
		LAMBDA_NO_COPY(DeviceBase);

		DeviceBase()
			: RefCountedObject<IDevice>(),
			m_Desc()
		{
		}
		~DeviceBase() = default;


		virtual const DeviceDesc& GetDesc() const override
		{
			return m_Desc;
		}
	protected:
		DeviceDesc m_Desc;
	};
}