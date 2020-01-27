#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	class IEventHandler;

	struct SWindowDesc
	{
		const char* pTitle	= nullptr;
		uint32 Width		= 0;
		uint32 Height		= 0;
	};

	class LAMBDA_API CCommonWindow
	{
	public:
		CCommonWindow() = default;
		virtual ~CCommonWindow() = default;

		LAMBDA_DECL_NO_COPY(CCommonWindow);

		virtual void Show() {}
		
		virtual void AddEventListener(IEventHandler*)		{}
		virtual void RemoveEventListener(IEventHandler*)	{}

		virtual uint32 GetWidth() const  { return 0; }
		virtual uint32 GetHeight() const { return 0; }
	};
}