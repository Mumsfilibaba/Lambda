#pragma once
#include <Defines.h>
#include <Types.h>
#if defined(LAMBDA_PLAT_WINDOWS)
	#include <vector>
	#include <memory>
	#include <wrl/client.h>
	#include <d3d12.h>

namespace Lambda
{
	//Keeping data about the allocation
	struct DX12Allocation
	{
		void* pCPU = nullptr;
		ID3D12Resource* pPageResource = nullptr;
		uint64 Offset = 0;
	};


	//Block for the linear allocator
	class DX12LinearAllocatorBlock final
	{
	public:
		LAMBDA_NO_COPY(DX12LinearAllocatorBlock);

		DX12LinearAllocatorBlock(ID3D12Device* pDevice, uint64 size);
		~DX12LinearAllocatorBlock();

		DX12Allocation Allocate(uint64 size, uint64 alignment);
		bool HasSpace(uint64 size, uint64 alignment) const;
		void Reset();

	private:
		void Init(ID3D12Device* pDevice, uint64 size);

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource;
		void* m_CPUPtr;
		D3D12_GPU_VIRTUAL_ADDRESS m_GPUPtr;
		uint64 m_Offset;
		uint64 m_Size;
	};


	//Allocator itself
	class DX12LinearAllocator final
	{
	public:
		LAMBDA_NO_COPY(DX12LinearAllocator);

		DX12LinearAllocator(ID3D12Device* pDevice, uint64 pageSize = MB(2));
		~DX12LinearAllocator();

		DX12Allocation Allocate(uint64 size, uint64 alignment);
		void Reset();

	private:
		void Init(ID3D12Device* pDevice);
		DX12LinearAllocatorBlock* AllocatePage();

	private:
		Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
		std::vector<std::unique_ptr<DX12LinearAllocatorBlock>> m_Pages;
		DX12LinearAllocatorBlock* m_CurrentPage;
		uint32 m_CurrentPageIndex;
		uint64 m_PageSize;
	};
}
#endif