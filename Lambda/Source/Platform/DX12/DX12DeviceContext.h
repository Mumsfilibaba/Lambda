#pragma once
#include "Graphics/Core/IDeviceContext.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Graphics/Core/DeviceObjectBase.h"
	#include "DX12LinearAllocator.h"
	#include "DX12LinearDescriptorAllocator.h"
	#include "DX12ResourceStateTracker.h"
	#include "DX12DescriptorCache.h"

namespace Lambda
{
	//-----------------
	//DX12DeviceContext
	//-----------------

	class DX12DeviceContext final : public DeviceObjectBase<DX12Device, IDeviceContext>
	{
		friend class DX12Device;
		friend class DX12CommandQueue;

	public:
		LAMBDA_NO_COPY(DX12DeviceContext);

		DX12DeviceContext(DX12Device* pDevice, DeviceContextType type, const DX12DescriptorHandle& nullSampler, const DX12DescriptorHandle& nullSRV, const DX12DescriptorHandle& nullUAV, const DX12DescriptorHandle& nullCBV);
		~DX12DeviceContext() = default;
	
		virtual void ClearRenderTarget(ITexture* pRenderTarget, float color[4]) override final;
		virtual void ClearDepthStencil(ITexture* pDepthStencil, float depth, uint8 stencil) override final;

		virtual void SetRendertargets(ITexture* const* ppRenderTargets, uint32 numRendertargets, ITexture* pDepthStencil) override final;
		virtual void SetViewports(const Viewport* pViewports, uint32 numViewports) override final;
		virtual void SetScissorRects(const Rectangle* pScissorRect, uint32 numRects) override final;
		virtual void SetVertexBuffers(IBuffer* const* pBuffers, uint32 numBuffers, uint32 slot) override final;
		virtual void SetIndexBuffer(IBuffer* pBuffer, Format format) override final;
		virtual void SetPipelineState(IPipelineState* pPipelineState) override final;
		virtual void SetShaderVariableTable(IShaderVariableTable* pVariableTable) override final;
		virtual void SetConstantBlocks(ShaderStage stage, uint32 offset, uint32 sizeInBytes, void* pData) override final;

		virtual void UpdateBuffer(IBuffer* pResource, const ResourceData* pData) override final;
		virtual void UpdateTexture(ITexture* pResource, const ResourceData* pData, uint32 mipLevel) override final;

		virtual void CopyBuffer(IBuffer* pDst, IBuffer* pSrc) override final;

		virtual void MapBuffer(IBuffer* pBuffer, uint32 mapFlags, void** ppData) override final;
		virtual void UnmapBuffer(IBuffer* pBuffer) override final;

		virtual void ResolveTexture(ITexture* pDst, uint32 dstMipLevel, ITexture* pSrc, uint32 srcMipLevel) override final;

		virtual void GenerateMipLevels(ITexture* pTexture) override final;

		virtual void Draw(uint32 vertexCount, uint32 startVertex) override final;
		virtual void DrawIndexed(uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation) override final;
		virtual void DrawInstanced(uint32 vertexCountPerInstance, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation) override final;
		virtual void DrawIndexedInstanced(uint32 indexCountPerInstance, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation) override final;

		virtual void ExecuteDefferedContext(IDeviceContext* pContext) override final;

		virtual void ResetQuery(IQuery* pQuery) override final;
		virtual void WriteTimeStamp(IQuery* pQuery, PipelineStage stage) override final;

		virtual void Flush() override final;
		virtual void ClearState() override final;

		virtual void SetName(const char* pName) override final;

		virtual DeviceContextType GetType() const override final;
		virtual void* GetNativeHandle() const override final;

		void Begin();
		void End();
		
		void TransitionBuffer(const IBuffer* pResource, ResourceState resourceState);
		void TransitionTexture(const ITexture* pResource, ResourceState resourceState, uint32 startMipLevel, uint32 numMipLevels);
	private:
		void Init(DeviceContextType type, const DX12DescriptorHandle& nullSampler, const DX12DescriptorHandle& nullSRV, const DX12DescriptorHandle& nullUAV, const DX12DescriptorHandle& nullCBV);
		void AllocateDescriptors();
		void InternalCopyAndSetDescriptors();
		void InternalSetResourceDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE hDest, D3D12_CPU_DESCRIPTOR_HANDLE hSrc, uint32 slot, uint32 range);
		void InternalSetSamplerDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE hDest, D3D12_CPU_DESCRIPTOR_HANDLE hSrc, uint32 slot);

		ID3D12CommandList* GetList() const;
	private:
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		m_Allocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList4>	m_List;
		DX12DescriptorHandle	 m_hResourceDescriptorTables[5];
		DX12DescriptorHandle	 m_hSamplerDescriptorTables[5];
		DX12DescriptorHandle	 m_NullDescriptors[4];
		DX12ResourceStateTracker m_ResourceTracker;
		DX12LinearAllocator		 m_BufferAllocator;
		DX12LinearAllocator		 m_TextureAllocator;
		DX12LinearDescriptorAllocator	m_ResourceAllocator;
		DX12LinearDescriptorAllocator	m_SamplerAllocator;
		DX12DescriptorCache	m_ResourceCache;
		DX12DescriptorCache	m_SamplerCache;
		uint32				m_SamplerDescriptorSize;
		uint32				m_ResourceDescriptorSize;
		DeviceContextType	m_Type;
	};


	inline ID3D12CommandList* DX12DeviceContext::GetList() const
	{
		return m_List.Get();
	}


	inline void DX12DeviceContext::InternalSetResourceDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE hDest, D3D12_CPU_DESCRIPTOR_HANDLE hSrc, uint32 slot, uint32 range)
	{
		//Get dest descriptor
		hDest.ptr += (uint64)m_ResourceDescriptorSize * (((uint64)range * 8) + slot);
		m_ResourceCache.PushDescriptor(hDest, hSrc);
	}


	inline void DX12DeviceContext::InternalSetSamplerDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE hDest, D3D12_CPU_DESCRIPTOR_HANDLE hSrc, uint32 slot)
	{
		//Get dest descriptor
		hDest.ptr += (uint64)m_SamplerDescriptorSize * slot;
		m_SamplerCache.PushDescriptor(hDest, hSrc);
	}
}
#endif
