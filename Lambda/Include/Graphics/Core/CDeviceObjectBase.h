#pragma once
#include "IDevice.h"
#include "IDeviceObject.h"
#include "Core/CRefCountBase.h"
#include <string>

namespace Lambda
{
	//-----------------
	//CDeviceObjectBase
	//-----------------

	template<typename TDeviceImpl, typename BaseInterface>
	class CDeviceObjectBase : public CRefCountBase<BaseInterface>
	{
	public:
        CDeviceObjectBase(TDeviceImpl* pDevice)
            : CRefCountBase<BaseInterface>(),
            m_pDevice(pDevice),
            m_Name("")
        {
            LAMBDA_ASSERT(pDevice != nullptr);
        }
        ~CDeviceObjectBase() = default;

        LAMBDA_NO_COPY(CDeviceObjectBase);

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
		std::string m_Name;
		TDeviceImpl* m_pDevice;
	};
}
