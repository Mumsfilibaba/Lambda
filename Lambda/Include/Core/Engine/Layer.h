#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	struct SSystemEvent;

	//------
	//CLayer
	//------
	class LAMBDA_API CLayer
	{
	public:
		CLayer(const char* pName);
		~CLayer() = default;

		LAMBDA_NO_COPY(CLayer);

		virtual bool DispatchEvent(const SSystemEvent& event);
	};
}