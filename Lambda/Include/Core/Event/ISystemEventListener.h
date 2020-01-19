#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    struct SSystemEvent;
    
    //--------------
    //CEventListener
    //--------------

	class LAMBDA_API ISystemEventListener
	{
	public:
		LAMBDA_INTERFACE(ISystemEventListener);
		virtual bool OnSystemEvent(const SSystemEvent& event) = 0;
	};
}
