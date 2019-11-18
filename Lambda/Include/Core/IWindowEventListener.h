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

		virtual void OnWindowMove(uint32 x, uint32 y) = 0;
		virtual void OnWindowFocusChanges(bool hasFocus) = 0;
		virtual void OnWindowResize(uint32 width, uint32 height) = 0;
		virtual void OnWindowClose() = 0;
	};
}