#pragma once
#include "IObject.h"

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

	class LAMBDA_API ISamplerState
	{
	public:
		LAMBDA_INTERFACE(ISamplerState);

		ISamplerState() = default;
		~ISamplerState() = default;

		virtual SamplerStateDesc GetDesc() const = 0;
        virtual void* GetNativeHandle() const = 0;
	};
}
