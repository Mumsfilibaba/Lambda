#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	//------------------
	//IHostEventListener
	//------------------

	class IHostEventListener
	{
	public:
		LAMBDA_INTERFACE(IHostEventListener);

		virtual void OnHostQuit() = 0;
	};
}