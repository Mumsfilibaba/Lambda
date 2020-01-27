#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	struct SEvent;

	class LAMBDA_API IEventHandler
	{
	public:
		LAMBDA_DECL_INTERFACE(IEventHandler);
		virtual bool OnEvent(const SEvent& event) = 0;
	};
}