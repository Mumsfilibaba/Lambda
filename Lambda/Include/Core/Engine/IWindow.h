#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	//-------
	//IWindow
	//-------
	class IWindow
	{
	public:
		LAMBDA_INTERFACE(IWindow);

        virtual bool HasFocus() const = 0;
        virtual bool IsMiniaturized() const = 0;
        virtual bool IsFullscreen() const = 0;
        
        virtual void SetFullscreen(bool bFullscreen) = 0;
        
		virtual uint32 GetWidth() const = 0;
		virtual uint32 GetHeight() const = 0;
		virtual void*  GetNativeHandle() const = 0;
	};
}