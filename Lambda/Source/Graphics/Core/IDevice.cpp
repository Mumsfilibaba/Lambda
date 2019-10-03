#include "LambdaPch.h"
#include "Graphics/Core/IDevice.h"
namespace Lambda
{
    //-------
    //IDevice
    //-------

	IDevice* IDevice::s_pInstance = nullptr;

	IDevice* IDevice::Get()
	{
        LAMBDA_ASSERT(s_pInstance != nullptr);
		return s_pInstance;
	}
}
