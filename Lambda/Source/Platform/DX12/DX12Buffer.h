#pragma once
#include "Graphics/Core/IBuffer.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include <wrl/client.h>
	#include "Graphics/Core/DeviceObjectBase.h"
	#include "DX12DescriptorHandle.h"

namespace Lambda
{
	class DX12Device;

	//----------
	//DX12Buffer
	//----------

	class DX12Buffer final : public DeviceObjectBase<DX12Device, IBuffer>
	{
		friend class DX12Device;
		friend class DX12DeviceContext;

	public:
		LAMBDA_NO_COPY(DX12Buffer);

		DX12Buffer(DX12Device* pDevice, const BufferDesc& desc);
		~DX12Buffer() = default;

		virtual void* GetNativeHandle() const override final;
		virtual const BufferDesc& GetDesc() const override final;
		
		void Map(void** ppMem);
		void Unmap();
	private:
		void Init(const BufferDesc& desc);
		
		void SetDescriporHandle(const DX12DescriptorHandle& hDescriptor);

		ID3D12Resource* GetResource() const;
		DX12DescriptorHandle GetDescriptorHandle() const;
		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAdress() const;
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Buffer;
		D3D12_VERTEX_BUFFER_VIEW m_VBV;
		D3D12_INDEX_BUFFER_VIEW m_IBV;
		D3D12_GPU_VIRTUAL_ADDRESS m_Adress;
		DX12DescriptorHandle m_hDescriptor;
		BufferDesc m_Desc;
	};


	inline ID3D12Resource* Lambda::DX12Buffer::GetResource() const
	{
		return m_Buffer.Get();
	}


	inline D3D12_VERTEX_BUFFER_VIEW DX12Buffer::GetVertexBufferView() const
	{
		return m_VBV;
	}


	inline D3D12_INDEX_BUFFER_VIEW DX12Buffer::GetIndexBufferView() const
	{
		return m_IBV;
	}


	inline DX12DescriptorHandle DX12Buffer::GetDescriptorHandle() const
	{
		return m_hDescriptor;
	}


	inline D3D12_GPU_VIRTUAL_ADDRESS DX12Buffer::GetGPUVirtualAdress() const
	{
		return m_Adress;
	}
}
#endif
