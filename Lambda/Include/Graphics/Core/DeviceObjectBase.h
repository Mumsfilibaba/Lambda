#pragma once
#include "IDevice.h"
#include "IDeviceObject.h"
#include <string>

namespace Lambda
{
	//----------------
	//DeviceObjectBase
	//----------------

	template<typename TDeviceImpl, typename BaseInterface>
	class DeviceObjectBase : public RefCountedObject<BaseInterface>
	{
	public:
		LAMBDA_NO_COPY(DeviceObjectBase);

		DeviceObjectBase(TDeviceImpl* pDevice) 
			: RefCountedObject<BaseInterface>(), 
			m_pDevice(pDevice), 
			m_Name("")
		{ 
			LAMBDA_ASSERT(pDevice != nullptr); 
		}
		~DeviceObjectBase() = default;


		virtual IDevice* GetDevice() const override
		{
			IDevice* pDevice = reinterpret_cast<IDevice*>(m_pDevice);
			pDevice->AddRef();
			return pDevice;
		}


		virtual void SetName(const char* pName) override
		{
			if (pName != nullptr)
				m_Name = std::string(pName);
		}
	protected:
		TDeviceImpl* m_pDevice;
		std::string m_Name;
	};
}
