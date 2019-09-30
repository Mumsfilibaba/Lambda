#pragma once
#include "IObject.h"

namespace Lambda
{
	class ITexture;

	//-------------------------
	//RenderPassAttachmentFlags
	//-------------------------

    enum RenderPassAttachmentFlags : uint32
    {
        RENDER_PASS_ATTACHMENT_FLAG_NONE = 0,
        RENDER_PASS_ATTACHMENT_FLAG_RESOLVE = (1 << 0),
    };
    
	//------------------------
	//RenderPassAttachmentDesc
	//------------------------

	struct RenderPassAttachmentDesc
	{
		ResourceFormat Format       = FORMAT_UNKNOWN;
        uint32 Flags                = RENDER_PASS_ATTACHMENT_FLAG_NONE;
        LoadOp LoadOperation        = LOAD_OP_UNKNOWN;
        StoreOp StoreOperation      = STORE_OP_UNKNOWN;
        ResourceState FinalState    = RESOURCE_STATE_UNKNOWN;
	};

	//--------------
	//RenderPassDesc
	//--------------

	struct RenderPassDesc
	{
		RenderPassAttachmentDesc RenderTargets[LAMBDA_MAX_RENDERTARGET_COUNT];
		uint32 NumRenderTargets = 0;
		RenderPassAttachmentDesc DepthStencil;
		uint32 SampleCount = 1;
	};

	//-----------
	//IRenderPass
	//-----------

	class LAMBDA_API IRenderPass : public IObject
	{
	public:
		LAMBDA_IOBJECT_INTERFACE(IRenderPass);
			
		virtual void SetRenderTargets(const ITexture* const* const ppRenderTargets, uint32 numRenderTargets, const ITexture* pDepthStencil) = 0;
		virtual void SetClearValues(float color[4], float depth, uint8 stencil) = 0;
		virtual void* GetNativeHandle() const = 0;
		virtual const RenderPassDesc& GetDesc() const = 0;
	};
}
