#pragma once
#include <Graphics/ICommandList.h>
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "DX12LinearAllocator.h"
	#include "DX12LinearDescriptorAllocator.h"
	#include "DX12ResourceStateTracker.h"

namespace Lambda
{
	class DX12CommandList final : public ICommandList
	{
		friend class DX12GraphicsDevice;

	public:
		LAMBDA_NO_COPY(DX12CommandList);

		DX12CommandList(ID3D12Device* pDevice, CommandListType type);
		~DX12CommandList();
	
		virtual void ClearRenderTarget(ITexture2D* pRenderTarget, float color[4]) override final;
		virtual void ClearDepthStencil(ITexture2D* pDepthStencil, float depth, uint8 stencil) override final;

		virtual void SetRenderTarget(ITexture2D* pRenderTarget, ITexture2D* pDepthStencil) override final;
		virtual void SetViewport(const Viewport& viewport) override final;
		virtual void SetScissorRect(const Math::Rectangle& scissorRect) override final;
		virtual void SetPrimtiveTopology(PrimitiveTopology topology) override final;
		virtual void SetGraphicsPipelineState(IGraphicsPipelineState* pPSO) override final;
		virtual void SetVertexBuffer(IBuffer* pBuffer, uint32 slot) override final; 

		virtual CommandListType GetType() const override final;

		virtual void TransitionResource(IBuffer* pResource, ResourceState resourceState) override final;
		virtual void TransitionResource(ITexture2D* pResource, ResourceState resourceState) override final;

		virtual void VSSetConstantBuffers(const IBuffer* const* ppBuffers, uint32 numBuffers, uint32 startSlot) override final;
		virtual void VSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot) override final;
		virtual void VSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot) override final;

		virtual void HSSetConstantBuffers(const IBuffer* const* ppBuffers, uint32 numBuffers, uint32 startSlot) override final;
		virtual void HSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot) override final;
		virtual void HSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot) override final;

		virtual void DSSetConstantBuffers(const IBuffer* const* ppBuffers, uint32 numBuffers, uint32 startSlot) override final;
		virtual void DSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot) override final;
		virtual void DSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot) override final;

		virtual void GSSetConstantBuffers(const IBuffer* const* ppBuffers, uint32 numBuffers, uint32 startSlot) override final;
		virtual void GSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot) override final;
		virtual void GSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot) override final;

		virtual void PSSetConstantBuffers(const IBuffer* const* ppBuffers, uint32 numBuffers, uint32 startSlot) override final;
		virtual void PSSetTextures(const ITexture2D* const* ppTextures, uint32 numTextures, uint32 startSlot) override final;
		virtual void PSSetSamplers(const ISamplerState* const* ppSamplerStates, uint32 numSamplers, uint32 startSlot) override final;

		virtual void UpdateBuffer(IBuffer* pResource, const ResourceData* pData) override final;
		virtual void UpdateTexture(ITexture2D* pResource, const ResourceData* pData, uint32 subresource) override final;

		virtual void CopyBuffer(IBuffer* pDst, IBuffer* pSrc) override final;

		virtual void DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation) override final;

		virtual void Close() override final;
		virtual void Reset() override final;
		
		virtual uint32 Release() override final;
		virtual uint32 AddRef() override final;

	private:
		void Init(ID3D12Device* pDevice, CommandListType type);
		void AllocateDescriptors();
		void InternalCopyAndSetDescriptors();
		void InternalSetResourceDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE hDest, D3D12_CPU_DESCRIPTOR_HANDLE hSrc, uint32 slot, uint32 range);
		void InternalSetSamplerDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE hDest, D3D12_CPU_DESCRIPTOR_HANDLE hSrc, uint32 slot);

		ID3D12CommandList* GetList() const;

	private:
		Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_Allocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4> m_List;
		
		DX12DescriptorHandle m_hResourceDescriptorTables[5];
		DX12DescriptorHandle m_hSamplerDescriptorTables[5];

		DX12ResourceStateTracker m_ResourceTracker;

		DX12LinearAllocator m_BufferAllocator;
		DX12LinearAllocator m_TextureAllocator;
		DX12LinearDescriptorAllocator m_ResourceAllocator;
		DX12LinearDescriptorAllocator m_SamplerAllocator;

		std::vector<UINT> m_DescriptorRangeCounts;
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_SrcDescriptorRanges;
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_DstDescriptorRanges;
		std::vector<UINT> m_SamplerDescriptorRangeCounts;
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_SamplerSrcDescriptorRanges;
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_SamplerDstDescriptorRanges;
		
		CommandListType m_Type;
		uint32 m_SamplerDescriptorSize;
		uint32 m_ResourceDescriptorSize;
		uint32 m_References;
	};


	inline ID3D12CommandList* DX12CommandList::GetList() const
	{
		return m_List.Get();
	}
}
#endif