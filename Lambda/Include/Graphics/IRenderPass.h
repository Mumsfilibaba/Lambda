#pragma once
#include "Defines.h"
#include "Types.h"

namespace Lambda
{
	class ITexture;


	struct RenderPassAttachmentDesc
	{
		ResourceFormat Format = FORMAT_UNKNOWN;
		LoadOp LoadOperation = LOAD_OP_UNKNOWN;
		StoreOp	StoreOperation = STORE_OP_UNKNOWN;
		ResourceState FinalState = RESOURCE_STATE_UNKNOWN;
	};


	struct RenderPassDesc
	{
		RenderPassAttachmentDesc RenderTargets[LAMBDA_MAX_RENDERTARGET_COUNT];
		uint32 NumRenderTargets = 0;
		RenderPassAttachmentDesc ResolveTargets[LAMBDA_MAX_RENDERTARGET_COUNT + 1];
		uint32 NumResolveTargets = 0;
		RenderPassAttachmentDesc DepthStencil;
		uint32 SampleCount = 1;
	};

	
	class LAMBDA_API IRenderPass
	{
	public:
		LAMBDA_INTERFACE(IRenderPass);

		IRenderPass() = default;
		~IRenderPass() = default;
	
		virtual void SetRenderTargets(const ITexture* const* const ppRenderTargets, uint32 numRenderTargets, const ITexture* pDepthStencil, const ITexture* const* const ppResolveTargets, uint32 numResolveTargets) = 0;
		virtual void SetClearValues(float color[4], float depth, uint8 stencil) = 0;
		virtual void* GetNativeHandle() const = 0;
	};
}