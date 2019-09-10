#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "DX12LinearAllocator.h"
	#include "Utilities/MathHelper.h"
namespace Lambda
{
	//Page
	DX12LinearAllocatorBlock::DX12LinearAllocatorBlock(ID3D12Device* pDevice, uint64 size)
		: m_Resource(nullptr),
		m_CPUPtr(nullptr),
		m_GPUPtr(0),
		m_Offset(0),
		m_Size(size)
	{
		LAMBDA_ASSERT(pDevice != nullptr);
		Init(pDevice, size);
	}


	DX12LinearAllocatorBlock::~DX12LinearAllocatorBlock()
	{
		m_Resource->Unmap(0, nullptr);
	}


	DX12Allocation DX12LinearAllocatorBlock::Allocate(uint64 size, uint64 alignment)
	{
		//Offset CPU pointer
		uint8* pBase = (uint8*)m_CPUPtr;

		m_Offset = Math::AlignUp<uint64>(m_Offset, alignment);

		DX12Allocation allocation = {};
		allocation.Offset = m_Offset;
		allocation.pCPU = (void*)(pBase + m_Offset);
		allocation.pPageResource = m_Resource.Get();

		m_Offset += Math::AlignUp<uint64>(size, alignment);
		return allocation;
	}


	bool DX12LinearAllocatorBlock::HasSpace(uint64 size, uint64 alignment) const
	{
		uint64 alignedOffset = Math::AlignUp<uint64>(m_Offset, alignment);
		uint64 alignedSize = Math::AlignUp<uint64>(size, alignment);
		return (alignedOffset + alignedSize) < m_Size;
	}


	void DX12LinearAllocatorBlock::Init(ID3D12Device* pDevice, uint64 size)
	{
		//Create page resource
		CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);
		CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		HRESULT hr = pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_Resource));
		if (FAILED(hr))
		{
			LOG_DEBUG_ERROR("DX12: Failed to create Linear-page. [DX12LinearAllocatorBlock]\n");
		}
		else
		{
			LOG_DEBUG_INFO("DX12: Created a Linear-page with the size '%d' bytes. [DX12LinearAllocatorBlock]\n", size);

			//Map if successfull
			m_Resource->Map(0, nullptr, &m_CPUPtr);
			m_GPUPtr = m_Resource->GetGPUVirtualAddress();
		}
	}


	//Allocator
	DX12LinearAllocator::DX12LinearAllocator()
		: m_Device(nullptr),
		m_CurrentPage(nullptr),
		m_Pages(),
		m_PageSize(0),
		m_CurrentPageIndex(0)
	{
	}


	DX12LinearAllocator::DX12LinearAllocator(ID3D12Device* pDevice, uint64 pageSize)
		: m_Device(nullptr),
		m_CurrentPage(nullptr),
		m_Pages(),
		m_PageSize(0),
		m_CurrentPageIndex(0)
	{
		Init(pDevice, pageSize);
	}


	void DX12LinearAllocator::Init(ID3D12Device* pDevice, uint64 pageSize)
	{
		LAMBDA_ASSERT(pDevice != nullptr);

		m_PageSize = pageSize;
		m_Device = pDevice;
		m_CurrentPage = AllocatePage();
	}
	
	
	DX12Allocation DX12LinearAllocator::Allocate(uint64 size, uint64 alignment)
	{
		DX12Allocation allocation;
		if (size >= m_PageSize)
		{
			LOG_DEBUG_ERROR("DX12: Failed to allocate. Requested size were '%d' while the total page size was '%d'.\n", size, m_PageSize);
		}
		else
		{
			//If requested size is within a page size we see if the current space got room or we allocate a new page
			if (!m_CurrentPage->HasSpace(size, alignment))
				m_CurrentPage = AllocatePage();

			//Make allocation
			allocation = m_CurrentPage->Allocate(size, alignment);
		}

		return allocation;
	}


	DX12LinearAllocatorBlock* DX12LinearAllocator::AllocatePage()
	{
		//Create a new page if nesessarry otherwise go to an already allocated page
		m_CurrentPageIndex++;
		if (m_CurrentPageIndex >= m_Pages.size())
		{
			m_Pages.push_back(std::make_unique<DX12LinearAllocatorBlock>(m_Device.Get(), m_PageSize));
			m_CurrentPageIndex = (uint32)m_Pages.size() - 1;
		}

		return m_Pages[m_CurrentPageIndex].get();
	}
}
#endif
