#pragma once
#include <Graphics/IBuffer.h>
#if defined(LAMBDA_PLAT_WINDOWS)
	#include <wrl/client.h>
	#include "DX12DescriptorHandle.h"

namespace Lambda
{
	class DX12Buffer final : public IBuffer
	{
		friend class DX12CommandList;

	public:
		LAMBDA_NO_COPY(DX12Buffer);

		DX12Buffer(ID3D12Device5* pDevice, const BufferDesc& desc);
		~DX12Buffer();

		virtual void Map(void** ppMem) override final;
		virtual void Unmap() override final;

		virtual uint32 Release() override final;
		virtual uint32 AddRef() override final;

	private:
		void Init(ID3D12Device5* pDevice, const BufferDesc& desc);
		
		void SetResourceState(D3D12_RESOURCE_STATES state) const;
		void SetDescriporHandle(const DX12DescriptorHandle& hDescriptor);

		DX12DescriptorHandle GetGPUDescriptor() const;
		ID3D12Resource* GetResource() const;
		D3D12_RESOURCE_STATES GetResourceState() const;
		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;
		D3D12_GPU_VIRTUAL_ADDRESS GetVirtualAdress() const;

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Buffer;
		mutable D3D12_RESOURCE_STATES m_State;
		D3D12_VERTEX_BUFFER_VIEW m_VBV;
		D3D12_INDEX_BUFFER_VIEW m_IBV;
		D3D12_GPU_VIRTUAL_ADDRESS m_Adress;
		DX12DescriptorHandle m_hDescriptor;
		
		BufferDesc m_Desc;
		uint32 m_References;
	};

	inline ID3D12Resource* Lambda::DX12Buffer::GetResource() const
	{
		return m_Buffer.Get();
	}

	inline D3D12_RESOURCE_STATES DX12Buffer::GetResourceState() const
	{
		return m_State;
	}

	inline D3D12_VERTEX_BUFFER_VIEW DX12Buffer::GetVertexBufferView() const
	{
		return m_VBV;
	}

	inline D3D12_INDEX_BUFFER_VIEW DX12Buffer::GetIndexBufferView() const
	{
		return m_IBV;
	}

	inline DX12DescriptorHandle DX12Buffer::GetGPUDescriptor() const
	{
		return m_hDescriptor;
	}

	inline D3D12_GPU_VIRTUAL_ADDRESS DX12Buffer::GetVirtualAdress() const
	{
		return m_Adress;
	}
}
#endif