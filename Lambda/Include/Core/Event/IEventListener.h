#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    struct SSystemEvent;
    
    //--------------
    //IEventListener
    //--------------
	class LAMBDA_API IEventListener
	{
	public:
		LAMBDA_INTERFACE(IEventListener);
		virtual bool OnEvent(const SSystemEvent& event) = 0;
	};
}
