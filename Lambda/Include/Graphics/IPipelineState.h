#pragma once
#include <IObject.h>

namespace Lambda
{
	class IShader;


	struct GraphicsPipelineStateDesc
	{
		IShader* pVertexShader = nullptr;
		IShader* pPixelShader = nullptr;
	};


	class LAMBDA_API IGraphicsPipelineState : public IObject
	{
	public:
		LAMBDA_INTERFACE(IGraphicsPipelineState);

		IGraphicsPipelineState() = default;
		~IGraphicsPipelineState() = default;
	};
}
