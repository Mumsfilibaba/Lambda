#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	struct SEvent;

	class LAMBDA_API IEventListener
	{
	public:
		LAMBDA_DECL_INTERFACE(IEventListener);
		virtual bool OnEvent(const SEvent& event) = 0;
	};
}
