#include <LambdaPch.h>
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "DX12DescriptorAllocator.h"
namespace Lambda
{
	DX12DescriptorAllocator::DX12DescriptorAllocator(ID3D12Device5* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 count, bool isShaderVisible)
		: m_Heap(nullptr),
		m_FreeList(),
		m_CPUStart(),
		m_GPUStart(),
		m_DescriptorSize(0),
		m_Count(0)
	{
		assert(pDevice != nullptr);
		Init(pDevice, type, count, isShaderVisible);
	}
	
	DX12DescriptorAllocator::~DX12DescriptorAllocator()
	{
	}
	
	void DX12DescriptorAllocator::Free(uint32 id)
	{
		m_FreeList.push_back(id);
	}

	uint32 DX12DescriptorAllocator::Allocate()
	{
		if (m_FreeList.empty())
		{
			LOG_DEBUG_ERROR("DX12: No more free descriptors.\n");
			return (uint32)-1;
		}

		uint32 id = m_FreeList.front();
		m_FreeList.front() = m_FreeList.back();
		m_FreeList.pop_back();
		return id;
	}
	
	D3D12_CPU_DESCRIPTOR_HANDLE DX12DescriptorAllocator::GetCPUHandle(uint32 index)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_CPUStart;
		handle.ptr += m_DescriptorSize * index;
		return handle;
	}
	
	D3D12_GPU_DESCRIPTOR_HANDLE DX12DescriptorAllocator::GetGPUHandle(uint32 index)
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle = m_GPUStart;
		handle.ptr += m_DescriptorSize * index;
		return handle;
	}

	void DX12DescriptorAllocator::Init(ID3D12Device5* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 count, bool isShaderVisible)
	{
		//Create heap
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Flags = isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 1;
		desc.NumDescriptors = count;
		desc.Type = type;

		HRESULT hr = pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_Heap));
		if (FAILED(hr))
		{
			LOG_DEBUG_ERROR("DX12: Failed to create Descriptor Heap.\n");
		}
		else
		{
			LOG_DEBUG_INFO("DX12: Created Descriptor Heap.\n");

			//Init values
			m_Count = count;
			m_DescriptorSize = pDevice->GetDescriptorHandleIncrementSize(type);
			
			//Get heap-startpoints
			m_CPUStart = m_Heap->GetCPUDescriptorHandleForHeapStart();
			m_GPUStart = m_Heap->GetGPUDescriptorHandleForHeapStart();

			//Init freelist
			m_FreeList.resize(m_Count);
			for (size_t i = 0; i < m_FreeList.size(); i++)
				m_FreeList[i] = (uint32)i;
		}
	}
}
#endif