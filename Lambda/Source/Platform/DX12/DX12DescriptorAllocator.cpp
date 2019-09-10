#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "DX12DescriptorAllocator.h"
namespace Lambda
{
	DX12DescriptorAllocator::DX12DescriptorAllocator()
		: m_Heap(nullptr),
		m_FreeList(),
		m_Start(),
		m_DescriptorSize(0),
		m_Count(0)
	{
	}


	DX12DescriptorAllocator::DX12DescriptorAllocator(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 count, bool isShaderVisible)
		: m_Heap(nullptr),
		m_FreeList(),
		m_Start(),
		m_DescriptorSize(0),
		m_Count(0)
	{
		Init(pDevice, type, count, isShaderVisible);
	}
	

	void DX12DescriptorAllocator::Free(const DX12DescriptorHandle& hDescriptor)
	{
		m_FreeList.push_back(hDescriptor.Index);
	}


	DX12DescriptorHandle DX12DescriptorAllocator::Allocate()
	{
		DX12DescriptorHandle descriptor;
		if (m_FreeList.empty())
		{
			descriptor.CPU.ptr = (SIZE_T)-1;
			descriptor.GPU.ptr = (SIZE_T)-1;
			descriptor.Index= (uint32)-1;

			LOG_DEBUG_ERROR("DX12: No more free descriptors. [DX12DescriptorAllocator]\n");
		}
		else
		{
			uint32 id = m_FreeList.front();
			m_FreeList.front() = m_FreeList.back();
			m_FreeList.pop_back();

			descriptor = DX12DescriptorHandle(m_Start, (uint64)m_DescriptorSize * id);
			descriptor.Index = id;
		}

		return descriptor;
	}


	void DX12DescriptorAllocator::Init(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 count, bool isShaderVisible)
	{
		LAMBDA_ASSERT(pDevice != nullptr);

		//Create heap
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Flags = isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 1;
		desc.NumDescriptors = count;
		desc.Type = type;

		HRESULT hr = pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_Heap));
		if (FAILED(hr))
		{
			LOG_DEBUG_ERROR("DX12: Failed to create Descriptor Heap. [DX12DescriptorAllocator]\n");
		}
		else
		{
			LOG_DEBUG_INFO("DX12: Created Descriptor Heap. [DX12DescriptorAllocator]\n");

			//Init values
			m_Count = count;
			m_DescriptorSize = pDevice->GetDescriptorHandleIncrementSize(type);
			
			//Get heap-startpoints
			m_Start.CPU = m_Heap->GetCPUDescriptorHandleForHeapStart();
			m_Start.GPU = m_Heap->GetGPUDescriptorHandleForHeapStart();

			//Init freelist
			m_FreeList.resize(m_Count);
			for (size_t i = 0; i < m_FreeList.size(); i++)
				m_FreeList[i] = (uint32)i;
		}
	}
}
#endif
