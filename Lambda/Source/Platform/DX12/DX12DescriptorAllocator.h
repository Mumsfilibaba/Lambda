#pragma once
#include "Defines.h"
#include "Types.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "DX12DescriptorHandle.h"
	#include <wrl/client.h>
	#include <vector>

namespace Lambda
{
	class DX12DescriptorAllocator final
	{
	public:
		LAMBDA_NO_COPY(DX12DescriptorAllocator);

		DX12DescriptorAllocator();
		DX12DescriptorAllocator(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 count, bool isShaderVisible);
		~DX12DescriptorAllocator() = default;

		void Init(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 count, bool isShaderVisible);
		
		void Free(const DX12DescriptorHandle& hDescriptor);
		DX12DescriptorHandle Allocate();
	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_Heap;
		std::vector<uint32> m_FreeList;
		DX12DescriptorHandle m_Start;
		uint32 m_DescriptorSize;
		uint32 m_Count;
	};
}
#endif
