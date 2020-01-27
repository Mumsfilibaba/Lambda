#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	struct SWindowDesc
	{
		const char* pTitle = nullptr;
		uint32 Width = 0;
		uint32 Height = 0;
	};

	class CGenericWindow
	{
	public:
		CGenericWindow() = default;
		~CGenericWindow() = default;

		LAMBDA_DECL_NO_COPY(CGenericWindow);

		virtual void Show() {}

		virtual uint32 GetHeight() const { return 0; }
		virtual uint32 GetWidth() const  { return 0; }
	};
}