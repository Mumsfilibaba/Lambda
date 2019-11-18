#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	//------------------
	//IHostEventListener
	//------------------

	class LAMBDA_API IHostEventListener
	{
	public:
		LAMBDA_INTERFACE(IHostEventListener);
		virtual void OnHostQuit(int32 exitCode) = 0;
	};
}