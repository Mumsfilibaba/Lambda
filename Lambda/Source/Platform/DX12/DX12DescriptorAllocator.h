#pragma once
#include <Defines.h>
#include <Types.h>
#if defined(LAMBDA_PLAT_WINDOWS)
	#include <vector>
	#include <wrl/client.h>
	#include <d3d12.h>

namespace Lambda
{
	class DX12DescriptorAllocator final
	{
	public:
		LAMBDA_NO_COPY(DX12DescriptorAllocator);

		DX12DescriptorAllocator(ID3D12Device5* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 count, bool isShaderVisible);
		~DX12DescriptorAllocator();

		void Free(uint32 id);
		uint32 Allocate();
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32 id);
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32 id);

	private:
		void Init(ID3D12Device5* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 count, bool isShaderVisible);

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_Heap;
		std::vector<uint32> m_FreeList;
		D3D12_CPU_DESCRIPTOR_HANDLE m_CPUStart;
		D3D12_GPU_DESCRIPTOR_HANDLE m_GPUStart;
		uint32 m_DescriptorSize;
		uint32 m_Count;
	};
}
#endif