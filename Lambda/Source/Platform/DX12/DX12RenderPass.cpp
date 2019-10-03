#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_WINDOWS)
    #include "DX12RenderPass.h"

namespace Lambda
{
	DX12RenderPass::DX12RenderPass(DX12Device* pDevice, const RenderPassDesc& desc)
		: DeviceObjectBase<DX12Device, IRenderPass>(pDevice),
		m_Desc()
	{
		Init(desc);
	}


	void DX12RenderPass::Init(const RenderPassDesc& desc)
	{
	}


	void DX12RenderPass::SetRenderTargets(const ITexture* const* const ppRenderTargets, uint32 numRenderTargets, const ITexture* pDepthStencil) 
	{
	}


	void DX12RenderPass::SetClearValues(float color[4], float depth, uint8 stencil)
	{
	}


	void* DX12RenderPass::GetNativeHandle() const
	{
		return nullptr;
	}


	const RenderPassDesc& DX12RenderPass::GetDesc() const
	{
		return m_Desc;
	}
}
#endif
