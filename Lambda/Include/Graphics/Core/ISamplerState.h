#pragma once
#include "IDeviceObject.h"

namespace Lambda
{
    //----------------
    //SamplerStateDesc
    //----------------
    
	struct SamplerStateDesc
	{
        SamplerAddressMode AdressMode	= SAMPLER_ADDRESS_MODE_REPEAT;
		float MinMipLOD					= 0.0f;
		float MaxMipLOD					= 0.0f;
		float MipLODBias				= 0.0f;
		float Anisotropy				= 1.0f;
	};

    //-------------
    //ISamplerState
    //-------------

	class LAMBDA_API ISamplerState : public IDeviceObject
	{
	public:
		LAMBDA_IOBJECT_INTERFACE(ISamplerState);

        virtual void* GetNativeHandle() const = 0;
		virtual const SamplerStateDesc& GetDesc() const = 0;
	};
}
