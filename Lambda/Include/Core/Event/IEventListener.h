#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    struct SEvent;
    
    //--------------
    //IEventListener
    //--------------
	class LAMBDA_API IEventListener
	{
	public:
		LAMBDA_INTERFACE(IEventListener);
		virtual bool OnEvent(const SEvent& event) = 0;
	};
}
