#pragma once
#include <Graphics/ICommandList.h>

#if defined(LAMBDA_PLAT_WINDOWS)
	#include <wrl/client.h>
	#include <d3d12.h>

namespace Lambda
{
	class DX12CommandList final : public ICommandList
	{
		friend class DX12GraphicsDevice;

	public:
		LAMBDA_NO_COPY(DX12CommandList);

		DX12CommandList(ID3D12Device5* pDevice, CommandListType type);
		~DX12CommandList();
	
		virtual void ClearRenderTarget(IRenderTarget* pRenderTarget, float color[4]) override final;
		virtual void ClearDepthStencil(ITexture2D* pDepthStencil, float depth, uint8 stencil) override final;

		virtual void SetRenderTarget(IRenderTarget* pRenderTarget, ITexture2D* pDepthStencil) override final;
		virtual void SetViewport(const Viewport& viewport) override final;
		virtual void SetScissorRect(const Math::Rectangle& scissorRect) override final;
		virtual void SetPrimtiveTopology(PrimitiveTopology topology) override final;
		virtual void SetGraphicsPipelineState(IGraphicsPipelineState* pPSO) override final;
		virtual void SetVertexBuffer(IBuffer* pBuffer, uint32 slot) override final; 

		virtual void TransitionResource(IBuffer* pResource, ResourceState resourceState) override final;
		virtual void TransitionResource(IRenderTarget* pRenderTarget, ResourceState resourceState) override final;

		virtual void CopyBuffer(IBuffer* pDst, IBuffer* pSrc) override final;

		virtual void DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation) override final;

		virtual void Close() override final;
		virtual void Reset() override final;
		
		virtual uint32 Release() override final;
		virtual uint32 AddRef() override final;

	private:
		void Init(ID3D12Device5* pDevice, CommandListType type);
		ID3D12CommandList* GetList() const;

		void InternalTransitionResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

	private:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_Allocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> m_List;
		uint32 m_References;
	};

	inline ID3D12CommandList* DX12CommandList::GetList() const
	{
		return m_List.Get();
	}
}
#endif