#pragma once
#include <IObject.h>

namespace Lambda
{
	struct SamplerDesc
	{

	};


	class LAMBDA_API ISamplerState : public IObject
	{
	public:
		LAMBDA_INTERFACE(ISamplerState);

		ISamplerState() = default;
		~ISamplerState() = default;

		virtual SamplerDesc GetDesc() const = 0;
	};
}