#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    class CEvent;
    
    //--------------
    //IEventListener
    //--------------

	class LAMBDA_API IEventListener
	{
	public:
		LAMBDA_INTERFACE(IEventListener);
		virtual bool OnEvent(const CEvent& event) = 0;
	};
}
