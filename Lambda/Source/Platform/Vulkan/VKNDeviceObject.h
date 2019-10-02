#pragma once
#include "Graphics/Core/IDeviceObject.h"
#include "Graphics/Core/IDevice.h"

namespace Lambda
{
    class VKNDevice;
    
    //---------------
    //VKNDeviceObject
    //---------------

    template<typename BaseInterface>
    class VKNDeviceObject : public RefCountedObject<BaseInterface>
    {
    public:
        LAMBDA_NO_COPY(VKNDeviceObject);
        
        VKNDeviceObject(VKNDevice* pDevice) : m_pDevice(pDevice) { LAMBDA_ASSERT(pDevice != nullptr); }
        ~VKNDeviceObject() = default;
        
        virtual IDevice* GetDevice() const override final
        {
            reinterpret_cast<IDevice*>(m_pDevice)->AddRef();
            return m_pDevice;
        }
        
    protected:
        VKNDevice* m_pDevice;
    };
}
