#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	//------------------
	//IHostEventListener
	//------------------

	class LAMBDA_API IEnvironmentEventListener
	{
	public:
		LAMBDA_INTERFACE(IEnvironmentEventListener);
		virtual void OnHostQuit(int32 exitCode) = 0;
	};
}