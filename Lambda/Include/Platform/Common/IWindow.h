#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	class IEventListener;

	struct SWindowDesc
	{
		const char* pTitle	= nullptr;
		uint32 Width		= 0;
		uint32 Height		= 0;
	};

	class LAMBDA_API IWindow
	{
	public:
		LAMBDA_DECL_INTERFACE(IWindow);

        virtual void Show() = 0;
		
        virtual void AddEventListener(IEventListener*) = 0;
        virtual void RemoveEventListener(IEventListener*) = 0;

		virtual uint32 GetWidth() const = 0;
		virtual uint32 GetHeight() const = 0;
        virtual void* GetNativeHandle() const = 0;
	};
}
