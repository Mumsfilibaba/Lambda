#pragma once
#include "Graphics/Core/IRenderPass.h"

namespace Lambda
{
	//------------------
	//DX12GraphicsDevice
	//------------------

	class DX12RenderPass : public RefCountedObject<IRenderPass>
	{
		LAMBDA_NO_COPY(DX12RenderPass);

		DX12RenderPass(const RenderPassDesc& desc);
		~DX12RenderPass() = default;

		virtual void	SetRenderTargets(const ITexture* const* const ppRenderTargets, uint32 numRenderTargets, const ITexture* pDepthStencil) override final;
		virtual void	SetClearValues(float color[4], float depth, uint8 stencil) override final;

		virtual void* GetNativeHandle() const override final;
		virtual const RenderPassDesc& GetDesc() const override final;
	private:
		void Init(const RenderPassDesc& desc);
	private:
		RenderPassDesc m_Desc;
	};
}