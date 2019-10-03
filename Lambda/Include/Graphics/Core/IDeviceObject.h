#pragma once
#include "IObject.h"

namespace Lambda
{
    class IDevice;
    
    //------------
    //DeviceObject
    //------------

    class LAMBDA_API IDeviceObject : public IObject
    {
    public:
        LAMBDA_IOBJECT_INTERFACE(IDeviceObject);
        
        virtual IDevice* GetDevice() const = 0;
    };
}
