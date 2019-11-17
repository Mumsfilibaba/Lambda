#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	//--------------------
	//IWindowEventListener
	//--------------------

	class IWindowEventListener
	{
	public:
		LAMBDA_INTERFACE(IWindowEventListener);

		virtual void OnWindowResize(uint32 width, uint32 height) = 0;
		virtual void OnWindowDestroy() = 0;
	};
}