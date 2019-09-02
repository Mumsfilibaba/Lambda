#pragma once
#include "IObject.h"

namespace Lambda
{
	struct SamplerDesc
	{
        SamplerAddressMode AdressMode = SAMPLER_ADDRESS_MODE_REPEAT;
	};


	class LAMBDA_API ISamplerState
	{
	public:
		LAMBDA_INTERFACE(ISamplerState);

		ISamplerState() = default;
		~ISamplerState() = default;

		virtual SamplerDesc GetDesc() const = 0;
        virtual void* GetNativeHandle() const = 0;
	};
}
