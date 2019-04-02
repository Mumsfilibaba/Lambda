#include <LambdaPch.h>
#include "DX12LinearDescriptorAllocator.h"

#if defined(LAMBDA_PLAT_WINDOWS)
namespace Lambda
{
	DX12LinearDescriptorAllocator::DX12LinearDescriptorAllocator(ID3D12Device5 * pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 count, bool isShaderVisible)
		: m_Device(nullptr),
		m_Heap(nullptr),
		m_First(),
		m_Count(count),
		m_Used(0)
	{
		assert(pDevice != nullptr);
		Init(pDevice, type, count, isShaderVisible);
	}

	DX12LinearDescriptorAllocator::~DX12LinearDescriptorAllocator()
	{
	}

	DX12DescriptorHandle DX12LinearDescriptorAllocator::Allocate(uint32 num)
	{
		DX12DescriptorHandle hDescriptor = m_First;
		hDescriptor.Index = m_Used;
		hDescriptor.CPU.ptr += m_Used * m_DescriptorSize;
		hDescriptor.GPU.ptr += m_Used * m_DescriptorSize;

		m_Used += num;
		return hDescriptor;
	}

	void DX12LinearDescriptorAllocator::Reset()
	{
		m_Used = 0;
	}

	void DX12LinearDescriptorAllocator::Init(ID3D12Device5* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32 count, bool isShaderVisible)
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
			LOG_DEBUG_ERROR("DX12: Failed to create Descriptor Heap. [DX12LinearDescriptorAllocator]\n");
		}
		else
		{
			LOG_DEBUG_INFO("DX12: Created Descriptor Heap. [DX12LinearDescriptorAllocator]\n");

			//Init values
			m_Count = count;
			m_DescriptorSize = pDevice->GetDescriptorHandleIncrementSize(type);

			//Get heap-startpoints
			m_First.CPU = m_Heap->GetCPUDescriptorHandleForHeapStart();
			m_First.GPU = m_Heap->GetGPUDescriptorHandleForHeapStart();
		}
	}
}
#endif