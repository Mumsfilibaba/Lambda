#include <LambdaPch.h>
#if defined(LAMBDA_PLAT_WINDOWS)
	#include <Math/Math.h>
	#include "DX12LinearAllocator.h"
namespace Lambda
{
	//Page
	DX12LinearAllocatorPage::DX12LinearAllocatorPage(ID3D12Device5* pDevice, uint64 size)
		: m_Resource(nullptr),
		m_CPUPtr(nullptr),
		m_GPUPtr(0),
		m_Offset(0),
		m_Size(size)
	{
		assert(pDevice != nullptr);
		
		Init(pDevice, size);
	}


	DX12LinearAllocatorPage::~DX12LinearAllocatorPage()
	{
		m_Resource->Unmap(0, nullptr);
	}


	DX12Allocation DX12LinearAllocatorPage::Allocate(uint64 size, uint64 alignment)
	{
		using namespace Math;

		//Offset CPU pointer
		uint8* pBase = (uint8*)m_CPUPtr;

		m_Offset = AlignUp<uint64>(m_Offset, alignment);

		DX12Allocation allocation = {};
		allocation.Offset = m_Offset;
		allocation.pCPU = (void*)(pBase + m_Offset);
		allocation.pPageResource = m_Resource.Get();

		m_Offset += AlignUp<uint64>(size, alignment);
		return allocation;
	}


	bool DX12LinearAllocatorPage::HasSpace(uint64 size, uint64 alignment) const
	{
		using namespace Math;
		uint64 alignedOffset = AlignUp<uint64>(m_Offset, alignment);
		uint64 alignedSize = AlignUp<uint64>(size, alignment);
		return (alignedOffset + alignedSize) < m_Size;
	}


	void DX12LinearAllocatorPage::Reset()
	{
		m_Offset = 0;
	}


	void DX12LinearAllocatorPage::Init(ID3D12Device5* pDevice, uint64 size)
	{
		//Create page resource
		CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);
		CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		HRESULT hr = pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_Resource));
		if (FAILED(hr))
		{
			LOG_DEBUG_ERROR("DX12: Failed to create Linear-page");
		}
		else
		{
			LOG_DEBUG_INFO("DX12: Created a Linear-page with the size '%d' bytes.\n", size);

			//Map if successfull
			m_Resource->Map(0, nullptr, &m_CPUPtr);
			m_GPUPtr = m_Resource->GetGPUVirtualAddress();
		}
	}


	//Allocator
	DX12LinearAllocator::DX12LinearAllocator(ID3D12Device5* pDevice, uint64 pageSize)
		: m_Device(nullptr),
		m_CurrentPage(nullptr),
		m_Pages(),
		m_PageSize(pageSize),
		m_CurrentPageIndex(0)
	{
		assert(pDevice != nullptr);
		Init(pDevice);
	}
	
	
	DX12LinearAllocator::~DX12LinearAllocator()
	{
	}
	
	
	DX12Allocation DX12LinearAllocator::Allocate(uint64 size, uint64 alignment)
	{
		DX12Allocation allocation;
		if (size >= m_PageSize)
		{
			LOG_DEBUG_ERROR("DX12: Failed to allocate. Reguested size were '%d' while the total page size was '%d'.\n", size, m_PageSize);
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


	void DX12LinearAllocator::Reset()
	{
		//Reset allocator and all pages
		m_CurrentPageIndex = 0;
		for (auto page = m_Pages.begin(); page < m_Pages.end(); page++)
			(*page)->Reset();
	}


	void DX12LinearAllocator::Init(ID3D12Device5* pDevice)
	{
		m_Device = pDevice;
		m_CurrentPage = AllocatePage();
	}


	DX12LinearAllocatorPage* DX12LinearAllocator::AllocatePage()
	{
		//Create a new page if nesessarry otherwise go to an already allocated page
		m_CurrentPageIndex++;
		if (m_CurrentPageIndex >= m_Pages.size())
		{
			m_Pages.push_back(std::make_unique<DX12LinearAllocatorPage>(m_Device.Get(), m_PageSize));
			m_CurrentPageIndex = (uint32)m_Pages.size() - 1;
		}

		return m_Pages[m_CurrentPageIndex].get();
	}
}
#endif