#pragma once
#include "IDevice.h"
#include "IDeviceObject.h"

namespace Lambda
{
	//----------------
	//DeviceObjectBase
	//----------------

	template<typename DeviceImpl, typename BaseInterface>
	class DeviceObjectBase : public RefCountedObject<BaseInterface>
	{
	public:
		LAMBDA_NO_COPY(DeviceObjectBase);

		DeviceObjectBase(DeviceImpl* pDevice) : RefCountedObject<BaseInterface>(), m_pDevice(pDevice) { LAMBDA_ASSERT(pDevice != nullptr); }
		~DeviceObjectBase() = default;

		virtual IDevice* GetDevice() const override final
		{
			IDevice* pDevice = reinterpret_cast<IDevice*>(m_pDevice);
			pDevice->AddRef();
			return pDevice;
		}

	protected:
		DeviceImpl* m_pDevice;
	};
}