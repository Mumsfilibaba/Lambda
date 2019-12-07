#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	//--------------------
	//ISystemEventListener
	//--------------------

    struct SSystemEvent;

	class LAMBDA_API ISystemEventListener
	{
	public:
		LAMBDA_INTERFACE(ISystemEventListener);
		virtual bool OnEvent(const SSystemEvent& event) = 0;
	};
}
