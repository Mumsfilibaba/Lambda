#pragma once
#include "Defines.h"
#include "Types.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include <wrl/client.h>
	#include "DX12DescriptorHandle.h"
namespace Lambda
{
	class DX12LinearDescriptorAllocator final
	{
		friend class DX12CommandList;

	public:
		LAMBDA_NO_COPY(DX12LinearDescriptorAllocator);

		DX12LinearDescriptorAllocator();
		DX12LinearDescriptorAllocator(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 count, bool isShaderVisible);
		~DX12LinearDescriptorAllocator() = default;

		void Init(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 count, bool isShaderVisible);
		
		//Allocates the number of descriptorhandles specified in 'num' and returns the first handle
		DX12DescriptorHandle Allocate(uint32 num);
		void Reset();

		uint32 GetDescriptorSize() const;

	private:
		ID3D12DescriptorHeap* GetHeap() const;

	private:
		Microsoft::WRL::ComPtr<ID3D12Device5> m_Device;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_Heap;
		DX12DescriptorHandle m_First;
		uint32 m_Used;
		uint32 m_DescriptorSize;
		uint32 m_Count;
	};


	inline void DX12LinearDescriptorAllocator::Reset()
	{
		m_Used = 0;
	}


	inline uint32 DX12LinearDescriptorAllocator::GetDescriptorSize() const
	{
		return m_DescriptorSize;
	}


	inline ID3D12DescriptorHeap* DX12LinearDescriptorAllocator::GetHeap() const
	{
		return m_Heap.Get();
	}
}
#endif
