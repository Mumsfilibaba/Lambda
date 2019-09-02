#pragma once
#include "Defines.h"
#include "Types.h"

namespace Lambda
{
	class ITexture2D;


	struct RenderPassAttachmentDesc
	{
		ResourceFormat Format = FORMAT_UNKNOWN;
		LoadOp LoadOperation = LOAD_OP_UNKNOWN;
		StoreOp	StoreOperation = STORE_OP_UNKNOWN;
	};


	struct RenderPassDesc
	{
		uint32 NumRenderTargets = 0;
		RenderPassAttachmentDesc RenderTargets[LAMBDA_MAX_RENDERTARGET_COUNT];
		RenderPassAttachmentDesc DepthStencil;
		uint32 SampleCount = 1;
	};

	
	class LAMBDA_API IRenderPass
	{
	public:
		LAMBDA_INTERFACE(IRenderPass);

		IRenderPass() = default;
		~IRenderPass() = default;
	
		virtual void SetRenderTargets(const ITexture2D* const* const ppRenderTargets, const ITexture2D* pDepthStencil) = 0;
		virtual void SetClearValues(float color[4], float depth, uint8 stencil) = 0;
		virtual void* GetNativeHandle() const = 0;
	};
}