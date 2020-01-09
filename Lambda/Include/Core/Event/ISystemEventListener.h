#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    struct SystemEvent;
    
    //--------------------
    //ISystemEventListener
    //--------------------
	class LAMBDA_API ISystemEventListener
	{
	public:
		LAMBDA_INTERFACE(ISystemEventListener);
		virtual bool OnSystemEvent(const SystemEvent& event) = 0;
	};
}
